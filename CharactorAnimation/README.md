# Character Animation with Direct3D

## Coding Conventions

| Type             | Prefix | Example                                    |
|------------------|--------|--------------------------------------------|
| Class Names      | N/A    | class SomeClass{ бн };                      |
| Function Names   | N/A    | void SomeFunction(int someParameter){ бн }  |
| Constant Names   | N/A    | const int CONSTANT_INTEGER = 32;           |
| Member Variables | m_     | int m_someInteger;                         |
| Global Variables | g_     | float g_globalFloat;                       |
| Static Variables | s_     | char s_staticChar;                         |
| Pointer Variable | p      | SomeClass* pPointerToObject;               |
| Member Pointer   | m_p    | float* m_pMemberPointerToFloat;            |

````c++
class SomeClass
{
	public:
		SomeClass();
		~SomeClass();
		void SomeFunction1(int someParameter);
		bool SomeFunction2();
	private:
		int m_memberVariable;
		float* m_pMemberPointer;
};
````


# CD code
http://www.delmarlearning.com/companions/content/1584505702/online_cd_contents/index.asp?isbn=1584505702