#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <pqxx/pqxx>
#include <fstream>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class AntycafeServer {
private:
    http_listener listener;
    std::string db_connection_string;

public:
    AntycafeServer(const std::string& url, const std::string& conn_str) 
        : listener(url), db_connection_string(conn_str) {
        
        listener.support(methods::GET, std::bind(&AntycafeServer::handle_get, this, std::placeholders::_1));
        listener.support(methods::POST, std::bind(&AntycafeServer::handle_post, this, std::placeholders::_1));
        listener.support(methods::OPTIONS, std::bind(&AntycafeServer::handle_options, this, std::placeholders::_1));
    }

    void handle_options(http_request request) {
        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
        response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
        request.reply(response);
    }

    void handle_get(http_request request) {
        auto path = request.relative_uri().path();
        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.headers().add(U("Content-Type"), U("application/json"));

        try {
            pqxx::connection conn(db_connection_string);
            
            if (path == "/api/tariffs") {
                pqxx::work txn(conn);
                auto result = txn.exec("SELECT id, name, price, description, type FROM tariffs");
                
                json::value json_response;
                json::value::array json_array;
                
                for (const auto& row : result) {
                    json::value item;
                    item[U("id")] = json::value::string(row["id"].as<std::string>());
                    item[U("name")] = json::value::string(row["name"].as<std::string>());
                    item[U("price")] = json::value::number(row["price"].as<int>());
                    item[U("description")] = json::value::string(row["description"].as<std::string>());
                    item[U("type")] = json::value::string(row["type"].as<std::string>());
                    json_array.push_back(item);
                }
                
                json_response[U("status")] = json::value::string(U("success"));
                json_response[U("data")] = json::value::array(json_array);
                response.set_body(json_response);
            }
            else if (path == "/api/cafes") {
                pqxx::work txn(conn);
                auto result = txn.exec("SELECT id, name, address, phone, capacity, description, work_hours FROM cafes");
                
                json::value json_response;
                json::value::array json_array;
                
                for (const auto& row : result) {
                    json::value item;
                    item[U("id")] = json::value::number(row["id"].as<int>());
                    item[U("name")] = json::value::string(row["name"].as<std::string>());
                    item[U("address")] = json::value::string(row["address"].as<std::string>());
                    item[U("phone")] = json::value::string(row["phone"].as<std::string>());
                    item[U("capacity")] = json::value::number(row["capacity"].as<int>());
                    item[U("description")] = json::value::string(row["description"].as<std::string>());
                    item[U("work_hours")] = json::value::string(row["work_hours"].as<std::string>());
                    json_array.push_back(item);
                }
                
                json_response[U("status")] = json::value::string(U("success"));
                json_response[U("data")] = json::value::array(json_array);
                response.set_body(json_response);
            }
            else {
                json::value json_response;
                json_response[U("status")] = json::value::string(U("error"));
                json_response[U("message")] = json::value::string(U("Endpoint not found"));
                response.set_body(json_response);
            }
        }
        catch (const std::exception& e) {
            json::value json_response;
            json_response[U("status")] = json::value::string(U("error"));
            json_response[U("message")] = json::value::string(U(e.what()));
            response.set_body(json_response);
            response.set_status_code(status_codes::InternalError);
        }
        
        request.reply(response);
    }

    void handle_post(http_request request) {
        auto path = request.relative_uri().path();
        http_response response(status_codes::OK);
        response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
        response.headers().add(U("Content-Type"), U("application/json"));

        request.extract_json().then([=, &response](pplx::task<json::value> task) {
            try {
                auto json_data = task.get();
                
                if (path == "/api/bookings") {
                    pqxx::connection conn(db_connection_string);
                    pqxx::work txn(conn);
                    
                    int cafe_id = json_data[U("cafe_id")].as_integer();
                    std::string name = json_data[U("name")].as_string();
                    std::string phone = json_data[U("phone")].as_string();
                    std::string date = json_data[U("date")].as_string();
                    std::string start_time = json_data[U("start_time")].as_string();
                    std::string end_time = json_data[U("end_time")].as_string();
                    int guests = json_data[U("guests")].as_integer();
                    std::string tariff = json_data[U("tariff_id")].as_string();
                    int cost = json_data[U("total_price")].as_integer();
                    std::string comment = json_data[U("comment")].as_string();
                    
                    // Рассчитать длительность
                    std::tm tm1 = {}, tm2 = {};
                    std::istringstream ss1(start_time + ":00");
                    std::istringstream ss2(end_time + ":00");
                    ss1 >> std::get_time(&tm1, "%H:%M:%S");
                    ss2 >> std::get_time(&tm2, "%H:%M:%S");
                    auto time1 = std::mktime(&tm1);
                    auto time2 = std::mktime(&tm2);
                    int duration = difftime(time2, time1) / 3600;
                    
                    // Вставить бронирование
                    std::string sql = "INSERT INTO bookings (cafe_id, name, phone, date, start_time, end_time, guests, tariff, duration, cost, comment, status) "
                                    "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, 'confirmed') "
                                    "RETURNING id";
                    
                    pqxx::params params;
                    params.append(cafe_id)
                          .append(name)
                          .append(phone)
                          .append(date)
                          .append(start_time)
                          .append(end_time)
                          .append(guests)
                          .append(tariff)
                          .append(duration)
                          .append(cost)
                          .append(comment);
                    
                    auto result = txn.exec_params(sql, params);
                    txn.commit();
                    
                    int booking_id = result[0][0].as<int>();
                    
                    json::value json_response;
                    json_response[U("status")] = json::value::string(U("success"));
                    json_response[U("booking_id")] = json::value::number(booking_id);
                    json_response[U("amount")] = json::value::number(cost);
                    response.set_body(json_response);
                }
                else {
                    json::value json_response;
                    json_response[U("status")] = json::value::string(U("error"));
                    json_response[U("message")] = json::value::string(U("Endpoint not found"));
                    response.set_body(json_response);
                }
            }
            catch (const std::exception& e) {
                json::value json_response;
                json_response[U("status")] = json::value::string(U("error"));
                json_response[U("message")] = json::value::string(U(e.what()));
                response.set_body(json_response);
                response.set_status_code(status_codes::InternalError);
            }
            
            request.reply(response);
        });
    }

    pplx::task<void> open() { return listener.open(); }
    pplx::task<void> close() { return listener.close(); }
};

int main() {
    std::string connection_string = "host=localhost port=5432 dbname=anticafe user=admin password=admin123";
    
    try {
        // Проверка подключения к БД
        pqxx::connection conn(connection_string);
        if (conn.is_open()) {
            std::cout << "Connected to database successfully!" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Database connection failed: " << e.what() << std::endl;
        return 1;
    }
    
    std::string url = "http://localhost:8080";
    AntycafeServer server(url, connection_string);
    
    std::cout << "Starting Antycafe server on " << url << std::endl;
    
    try {
        server.open().wait();
        std::cout << "Press Enter to exit..." << std::endl;
        std::string line;
        std::getline(std::cin, line);
        server.close().wait();
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}