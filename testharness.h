#ifndef STORAGE_LEVELDB_UTIL_TESTHARNESS_H_
#define STORAGE_LEVELDB_UTIL_TESTHARNESS_H_

#include<sstream>
#include"slice.h"
#include"status.h"

namespace leveldb 
{
	namespace test
	{
		// Run some of the tests registered by the TEST() macro.  If the
		// environment variable "LEVELDB_TESTS" is not set, runs all tests.
		// Otherwise, runs only the tests whose name contains the value of
		// "LEVELDB_TESTS" as a substring.  E.g., suppose the tests are:
		//    TEST(Foo, Hello) { ... }
		//    TEST(Foo, World) { ... }
		// LEVELDB_TESTS=Hello will run the first test
		// LEVELDB_TESTS=o     will run both tests
		// LEVELDB_TESTS=Junk  will run no tests
		//
		// Returns 0 if all tests pass.
		// Dies or returns a non-zero value if some test fails.
		extern int runAllTests();

		// Return the directory to use for temporary storage.
		extern std::string tmpDir();

		extern int randomSeed();
		class Tester
		{
		private:
			bool ok_;
			const char* fname_;
			int line_;
			std::stringstream ss_;
		public:
			Tester(const char* f, int l)
				:ok_(true), fname_(f), line_(l)
			{
			}
			~Tester()
			{
				if (!ok_)
				{
					fprintf(stderr, "%s:%d:%s\n", fname_, line_, ss_.str().c_str());
					exit(1);
				}
			}
			Tester& is(bool b, const char* msg)
			{
				if (!b)
				{
					ss_ << " assertion failure " << msg;
					ok_ = false;
				}
				return *this;
			}
			Tester& isOk(const Status& s)
			{
				if (!s.ok())
				{
					ss_ << " " << s.toString();
					ok_ = false;
				}
				return *this;
			}

#define BINARY_OP(name, op)											\
			template <class X, class Y>								\
			Tester& name(const X& x, const Y& y)					\
			{														\
				if( !(x op y))										\
				{													\
					ss_ << " failed: "<< x << (" " #op " ")<< y;	\
					ok_ = false;									\
				}													\
				return *this;										\
			}
				BINARY_OP(isEq, == )
				BINARY_OP(isNe, != )
				BINARY_OP(isGe, >= )
				BINARY_OP(isGt, >)
				BINARY_OP(isLe, <= )
				BINARY_OP(isLt, <)
#undef BINARY_OP

				//错误发生时输出错误信息
				template <class V>
			Tester& operator<<(const V& value)
			{
				if (!ok_)
				{
					ss_ << " " << value;
				}
				return *this;
			}
		};

#define ASSERT_TRUE(c) ::leveldb::test::Tester(__FILE__, __LINE__).is((c), #c)
#define ASSERT_OK(c) ::leveldb::test::Tester(__FILE__, __LINE__).isOk((c))
#define ASSERT_EQ(a,b) ::leveldb::test::Tester(__FILE__, __LINE__).isEq((a),(b))
#define ASSERT_NE(a,b) ::leveldb::test::Tester(__FILE__, __LINE__).isNe((a),(b))
#define ASSERT_GE(a,b) ::leveldb::test::Tester(__FILE__, __LINE__).isGe((a),(b))
#define ASSERT_GT(a,b) ::leveldb::test::Tester(__FILE__, __LINE__).isGt((a),(b))
#define ASSERT_LE(a,b) ::leveldb::test::Tester(__FILE__, __LINE__).isLe((a),(b))
#define ASSERT_LT(a,b) ::leveldb::test::Tester(__FILE__, __LINE__).isLt((a),(b))


		#define TCONCAT(a,b) TCONCAT1(a,b)
		#define TCONCAT1(a,b) a##b

		#define TEST(base, name)                 \
			class TCONCAT(_Test_, name): public base \
			{                                        \
				public:								 \
					void _run();                     \
					static void _runIt()             \
					{                                \
						TCONCAT(_Test_,name) t;      \
						t._run();                    \
					}                                \
			};                                       \
			bool TCONCAT(_Test_ignored_, name) =     \
			  ::leveldb::test::registerTest(#base, #name, &TCONCAT(_Test_, name)::_runIt); \
			void TCONCAT(_Test_, name)::_run()

		//回调函数
		extern bool registerTest(const char* base, const char* name, void(*func)());
	}
}
#endif
