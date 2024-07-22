#include <sstream>
#include "http_req.h"

class client_http {
private:
    string username, password, access_token;
    http_req req;
    bool logged_in;
    char *rc, **body_data, **cookies;
public:

    client_http() {
        logged_in = false;
        cookies = nullptr;
    }
    bool is_valid_username(const string &username) {
        return username.find('\'') == string::npos; // Ensure username is not empty
    }
    void get_credentials() {
        cout << "username=";
        getline(cin, username);
        getline(cin, username);
        cout << "password=";
        cin >> password;
    }
    bool is_logged_in() {
        if (!logged_in)
            cout << "Error: Access denied\n";
        return logged_in;
    }
    string get_JSON_credentials() {
        return "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";
    }
    char *get_JSON_book() {
        string title, author, genre, publisher, page_count;
        cout << "title=";
        getline(cin, title);
        getline(cin, title);
        cout << "author=";
        getline(cin, author);
        cout << "genre=";
        getline(cin, genre);
        cout << "publisher=";
        getline(cin, publisher);
        cout << "page_count=";
        getline(cin, page_count);

        if (!page_count.empty() && !all_of(page_count.begin(), page_count.end(), ::isdigit)) {
            cout << "Error: Invalid page_count\n";
            return nullptr;
        }
        if (author.empty() || genre.empty() || publisher.empty()) {
            cout << "Error: Invalid author\n";
            return nullptr;
        }

        json book = {
            {"title", title},
            {"author", author},
            {"genre", genre},
            {"publisher", publisher},
            {"page_count", stoi(page_count)}
        };

        return strdup(book.dump().c_str());
    }
    void register_client() {
        req.open_connection();

        body_data = (char **) calloc(1, sizeof(char *));
        body_data[0] = (char *) calloc(LINE_SIZE, sizeof(char));

        strcpy(body_data[0], get_JSON_credentials().c_str());
        strcat(body_data[0], "\0");

        char *message = req.POST(REGISTER, body_data, 1, NULL, 0, NULL, 0);
        req.send_to_server(message);
        rc = req.receive_from_server();

        MSG(strstr(rc, "ok"), "User registration");

        free(body_data[0]);
        free(body_data);

        req.close_connection();
    }
    void login_client() {
        if (logged_in) {
            cout << "Error: User already logged in\n";
            return;
        }
        req.open_connection();
        body_data = (char **) calloc(1, sizeof(char *));
        body_data[0] = (char *) calloc(LINE_SIZE, sizeof(char));

        strcpy(body_data[0], get_JSON_credentials().c_str());
        strcat(body_data[0], "\0");

        char *message = req.POST(LOGIN, body_data, 1, NULL, 0, NULL, 0);
        req.send_to_server(message);
        rc = req.receive_from_server();

        cookies = (char **) calloc(1, sizeof(char *));
        cookies[0] = (char *) calloc(LINE_SIZE, sizeof(char));

        string cook = string(rc);
        cook = cook.substr(cook.find("Set-Cookie: ") + 12);
        cook = cook.substr(0, cook.find("\r\n"));
        strcpy(cookies[0], cook.c_str());

        cout << "Cookie: " << cook << endl;

        if (strstr(rc, "ok")) {
            logged_in = true;
        }

        MSG(logged_in, "User login");

        free(body_data[0]);
        free(body_data);
        free(rc);
        req.close_connection();
    }
    void access_library() {
        req.open_connection();

        char *message = req.GET(ACCESS, NULL, cookies, 1, NULL, 0);
        req.send_to_server(message);
        rc = req.receive_from_server();

        json token = json::parse(strstr(rc, "{"));

        if (!token["token"].empty()) {
            access_token = token["token"];
            MSG(true, "Access granted");
        } else {
            MSG(false, "Access denied");
        }

        free(rc);
        req.close_connection();
    }
    void list_summary() {
        req.open_connection();

        body_data = (char **) calloc(1, sizeof(char *));
        body_data[0] = (char *) calloc(LINE_SIZE, sizeof(char));

        strcat(body_data[0], "Authorization: Bearer ");
        strcat(body_data[0], access_token.c_str());

        char *message = req.GET(BOOK, NULL, cookies, 1, body_data, 1);

        cout << message << endl;

        req.send_to_server(message);
        rc = req.receive_from_server();

        cout << rc << endl;

        if (strstr(rc, "error")) {
            MSG(false, "Access denied");
        } else {
            json books = json::parse(strstr(rc, "["));
            MSG(true, ("\n" + books.dump(2)).c_str());
        }

        free(rc);
        free(body_data[0]);
        free(body_data);
        req.close_connection();
    }
    void list_book(string id) {
        req.open_connection();

        body_data = (char **) calloc(1, sizeof(char *));
        body_data[0] = (char *) calloc(LINE_SIZE, sizeof(char));

        strcat(body_data[0], "Authorization: Bearer ");
        strcat(body_data[0], access_token.c_str());
        char *message = req.GET(BOOK, &id, cookies, 1, body_data, 1);
        req.send_to_server(message);
        rc = req.receive_from_server();

        if (strstr(rc, "error")) {
            MSG(false, "Access denied or invalid ID");
        } else {
            json book = json::parse(strstr(rc, "{"));
            MSG(true, ("\n" + book.dump(2)).c_str());
        }

        free(rc);
        free(body_data[0]);
        free(body_data);
        req.close_connection();
    }
    void add_book(char *JSON) {
        req.open_connection();

        body_data = (char **) calloc(1, sizeof(char *));
        body_data[0] = (char *) calloc(LINE_SIZE, sizeof(char));

        strcat(body_data[0], "Authorization: Bearer ");
        strcat(body_data[0], access_token.c_str());

        char **header_data = (char **) calloc(1, sizeof(char *));
        header_data[0] = (char *) calloc(LINE_SIZE, sizeof(char));
        strcat(header_data[0], JSON);
        strcat(header_data[0], "\0");

        char *message = req.POST(BOOK, header_data, 1, cookies, 1, body_data, 1);
        req.send_to_server(message);
        rc = req.receive_from_server();

        MSG(strstr(rc, "ok"), "Book adding");

        free(rc);
        free(body_data[0]);
        free(body_data);
        free(header_data[0]);
        free(header_data);
        req.close_connection();
    }
    void delete_book(string id) {
        if (!is_logged_in()) return;

        req.open_connection();

        body_data = (char **) calloc(1, sizeof(char *));
        body_data[0] = (char *) calloc(LINE_SIZE, sizeof(char));

        strcat(body_data[0], "Authorization: Bearer ");
        strcat(body_data[0], access_token.c_str());

        char *message = req.DELETE(BOOK, &id, cookies, 1, body_data, 1);
        req.send_to_server(message);
        rc = req.receive_from_server();

        MSG(!strstr(rc, "404"), "Book deletion");

        free(rc);
        free(body_data[0]);
        free(body_data);
        req.close_connection();
    }
    void logout() {
        req.open_connection();

        char *message = req.GET(LOGOUT, NULL, cookies, 1, NULL, 0);
        req.send_to_server(message);
        rc = req.receive_from_server();

        if (strstr(rc, "ok")) {
            MSG(logged_in, "User logout");
            logged_in = false;
        } else {
            MSG(false, "User not logged in");
        }

        free(rc);
        req.close_connection();
    }
    void run_client() {
        bool running = true;
        logged_in = false;
        string command;
        while (running) {
            cin >> command;
            if (command == "register") {
                get_credentials();
                if (!is_valid_username(username)) {
                    cout << "Error: Invalid username\n";
                    continue;
                }
                register_client();
            } else if (command == "login") {
                get_credentials();
                if (!is_valid_username(username)) {
                    cout << "Error: Invalid username\n";
                    continue;
                }
                login_client();
            } else if (command == "enter_library") {
                access_library();
            } else if (command == "get_books") {
                list_summary();
            } else if (command == "get_book") {
                if (!is_logged_in()) continue;
                string id;
                cout << "id=";
                cin >> id;
                list_book(id);
            } else if (command == "add_book") {
                if (!is_logged_in()) continue;
                char *JSON = get_JSON_book();
                if (JSON == nullptr) {
                    continue;
                }
                add_book(JSON);
            } else if (command == "delete_book") {
                if (!is_logged_in()) continue;
                string id;
                cout << "id=";
                cin >> id;
                delete_book(id);
            } else if (command == "logout") {
                logout();
            } else if (command == "exit") {
                running = false;
            }
        }
    }
};

int main () {
    client_http client = client_http();

    client.run_client();

    return 0;
}