#include "rose/database/database.h"

#include <libpq-fe.h>

namespace Rose::Database {

void
PgConnDeleter::operator()(PGconn* c) {
    PQfinish(c);
};

void
PgResultDeleter::operator()(PGresult* r) {
    PQclear(r);
};

bool
Client::connect(const std::string& conn_string) {
    this->conn = PgConnPtr{PQconnectdb(conn_string.c_str())};

    if (PQstatus(this->conn.get()) != CONNECTION_OK) {
        return false;
    }

    return true;
}

std::string
Client::last_error_message() {
    return PQerrorMessage(this->conn.get());
}

}