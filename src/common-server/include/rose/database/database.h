#pragma once

#include <memory>
#include <optional>
#include <string>

// Forward declarations for libpq types
struct pg_conn;
struct pg_result;
typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;

namespace Rose::Database {

struct PgConnDeleter {
    void operator()(PGconn* c);
};

struct PgResultDeleter {
    void operator()(PGresult* r);
};

using PgConnPtr = std::unique_ptr<PGconn, PgConnDeleter>;
using PgResultPtr = std::unique_ptr<PGresult, PgResultDeleter>;

/// Database client
class Client {
public:
    PgConnPtr conn;

public:
    Client() = default;
    ~Client() = default;
    Client(Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;

    /// Connect using a connection string
    bool connect(const std::string& conn_string);

    /// Get the last error message  from the connection
    std::string last_error_message();
};
}