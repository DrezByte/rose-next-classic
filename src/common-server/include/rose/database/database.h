#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

// Forward declarations for libpq types
struct pg_conn;
struct pg_result;
typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;

// Forward declaration for Rose::Util
namespace Rose::Util {
using DateTime = std::chrono::time_point<std::chrono::system_clock>;
}

namespace Rose::Database {

/// Builds a list of bind params as a string
/// e.g. param_list(4) => "$1, $2, $3, $4"
std::string param_list(size_t count);

struct PgConnDeleter {
    void operator()(PGconn* c);
};

struct PgResultDeleter {
    void operator()(PGresult* r);
};

using PgConnPtr = std::unique_ptr<PGconn, PgConnDeleter>;
using PgResultPtr = std::unique_ptr<PGresult, PgResultDeleter>;

using QueryResultRow = std::vector<std::string>;
using QueryResultRows = std::vector<QueryResultRow>;

enum class QueryParamType {
    Null,
    String,
};

/// QueryParam that contains extra information about the parameter
struct QueryParam {
    QueryParamType type;
    std::vector<uint8_t> data;

    QueryParam();
    QueryParam(const std::string& s);
    QueryParam(const char* s);
};

/// Result from a query
class QueryResult {
public:
    PgResultPtr res;

    size_t row_count = 0;
    size_t col_count = 0;
    std::vector<std::string> data;

public:
    QueryResult() = delete;
    QueryResult(PGresult* res);

    /// Returns true if the query executed successfully
    bool is_ok();

    /// Returns the error message for this result
    const char* error_message();

    /// Get all rows from the query result
    QueryResultRows rows();

    /// Get a row from the query result by index
    QueryResultRow row(size_t idx);

    /// Get a value from the query result by row and column index
    std::string get_string(size_t row_idx, size_t col_idx);
    int32_t get_int32(size_t row_idx, size_t col_idx);
    Rose::Util::DateTime get_datetime(size_t row_idx, size_t col_idx);
    bool get_null(size_t row_idx, size_t col_idx);
};

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
    const char* last_error_message();

    /// Execute a query and get the results using string params
    QueryResult query(const std::string& statement, const std::vector<std::string>& params);

    /// Execute a query and get the results using binary params
    QueryResult queryb(const std::string& statement, const std::vector<QueryParam>& params);
};
} // namespace Rose::Database