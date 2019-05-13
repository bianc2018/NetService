#ifndef DB_DBBASE_H
#define DB_DBBASE_H

extern "C"
{
	#include "sql/sqlite3.h"
}
#include "json/json.h"
#include "log/log.hpp"

#include <string>
#include <vector>
#include <mutex>

namespace db
{
	class DbStmt
	{
	public:
		//
		DbStmt();
		DbStmt(const DbStmt&s);
		DbStmt(sqlite3 *db_ptr,const std::string&sql);
		~DbStmt();
		
		template<typename ...Types>
		int bind_get_row(Json &json,Types && ...args);

		template<typename ...Types>
		int bind_get_rows(Json &json, Types && ...args);

		template<typename ...Types>
		int bind_exec(Types && ...args);

		void free();
		//true ok
		bool is_ok();
	private:
		template<typename ...Types>
		DbStmt& bind(Types && ... args);

		int get_row(Json&json);
		int get_rows(Json&json);

		int exec();

		template<typename T, typename ...Types>
		void bind_args(int pos,T n, Types && ...args);
		template<typename T>
		void bind_args(int pos,T n);
		void bind_args(int pos);

		void sql_bind(int pos,const std::string &value);
		void sql_bind(int pos,const  int& value);
		void sql_bind(int pos, const  double& value);
		void sql_bind(int pos, const  sqlite3_int64& value);
		void sql_bind(int pos, const  size_t& value);
	private:
		sqlite3_stmt*ptr_;
		sqlite3 *db_ptr_;
		std::mutex lock_;
	};
	
	class DbBase
	{
	public:
		int connect(const std::string &url, int busy_time_out = 1000);

		//0 full¡¢ 1 normal¡¢2 off Ð´Í¬²½
		int set_synchronous(int i=0);
		DbStmt new_stmt(const std::string &str);
		int exec(const std::string &sql);
		int close();

		~DbBase();
	protected:
		std::map<std::string, db::DbStmt> stmt_map_;
		sqlite3 *db_ptr_ = nullptr;
		
	};

	
	
	template<typename ...Types>
	inline int DbStmt::bind_get_row(Json & json, Types && ...args)
	{
		std::lock_guard<std::mutex> lock(lock_);
		bind(std::forward<Types>(args)...);
		return get_row(json);
	}

	template<typename ...Types>
	inline int DbStmt::bind_get_rows(Json & json, Types && ...args)
	{
		std::lock_guard<std::mutex> lock(lock_);
		bind(std::forward<Types>(args)...);
		return get_rows(json);
	}

	template<typename ...Types>
	inline int DbStmt::bind_exec(Types && ...args)
	{
		std::lock_guard<std::mutex> lock(lock_);
		bind(std::forward<Types>(args)...);
		return exec();
	}

	template<typename ...Types>
	inline DbStmt& DbStmt::bind(Types && ...args)
	{
		sqlite3_reset(ptr_);
		bind_args(1, std::forward<Types>(args)...);
		return *this;
	}
	template<typename T, typename ...Types>
	inline void DbStmt::bind_args(int pos, T n, Types && ...args)
	{
		sql_bind(pos, n);
		pos += 1;
		bind_args(pos, std::forward<Types>(args)...);
	}
	inline void DbStmt::bind_args(int pos)
	{
		return;
	}
	template<typename T>
	inline void DbStmt::bind_args(int pos, T n)
	{
		sql_bind(pos, n);
	}
}
#endif //!DB_DBBASE_H
