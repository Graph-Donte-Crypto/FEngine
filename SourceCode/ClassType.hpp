#ifndef FENGINE_SC_ClassType
#define FENGINE_SC_ClassType

struct ClassType {

	const char * ptr = nullptr;

	#define Type(type) type = #type

	static constexpr const char
		* Type(Null),
		* Type(Basic),
		* Type(Entity),
		* Type(Block),
		* Type(BlockTeleporter),
		* Type(Player),
		* Type(NetworkPlayer)
		;

	#undef Type

	ClassType(const char * p) : ptr(p) {}
	operator const char * () const { return ptr;}
	ClassType & operator = (const char * p) { ptr = p; return *this;}
	bool operator == (const ClassType & type) const { return ptr == type.ptr;};
	bool operator != (const ClassType & type) const { return ptr != type.ptr;};
};

#endif