
#define NDEBUG

#include <string>

class Color;

class bad_index : std::exception {};
class bad_pointer : std::exception {};

template <typename CharT, typename Traits = std::char_traits<CharT>>
class m_basic_string {
public:

	class string_rep {
		friend class m_basic_string;
	public:
		string_rep(void);
		string_rep(const CharT c);	
		string_rep(size_t len, const CharT* s);

		string_rep(const string_rep&) = delete;
		string_rep& operator=(const string_rep&) = delete;

		~string_rep();

		void colorize(unsigned __int8, unsigned __int8, unsigned __int8);

		//pseudo-copying constructor
		string_rep* get_own_copy();
		//pseudo assignment
		void assign(size_t len, const CharT* s);

		inline size_t use_count() const
		{
			return use_count_;
		};

		inline size_t len() const
		{
			return len_;
		};

		inline CharT* str() const
		{
			return str_;
		};

		inline const Color& color() const {
			return color_;
		};

	private:

		size_t len_;
		CharT* str_;
		size_t use_count_;

		Color& color_;
	};

	class Proxy
	{
		friend class m_basic_string;
		
		friend std::basic_ostream<CharT>&
			operator<<(std::basic_ostream<CharT>& os, const typename m_basic_string<CharT>::Proxy& proxy)
		{
			Color color = proxy.b_str_.color();
			
			return os << Color::rgb_sequence
				<< std::to_string(color.get_r()) << ';'
				<< std::to_string(color.get_g()) << ';'
				<< std::to_string(color.get_b()) << 'm'
				<< proxy.b_str_.c_str()[proxy.pos_] << Color::reset;
		};
		
	public:
		Proxy(m_basic_string& s, size_t pos);
		~Proxy() = default;	
		
		inline operator CharT() const;
		Proxy& operator= (const CharT value);

	private:
		m_basic_string&       b_str_;
		size_t                pos_;
	};

	//concatenation 
	m_basic_string(const CharT* const l, const CharT* const r);
	m_basic_string(const CharT* const l, const CharT r);
	m_basic_string(const CharT const l, const CharT* r);

	//converting
	m_basic_string(void);
	m_basic_string(const CharT c);
	m_basic_string(const CharT* const s);
	m_basic_string(std::basic_string<CharT>& s);

	//copy
	m_basic_string(const m_basic_string& c);
	m_basic_string& operator=(const m_basic_string& s)&;
	m_basic_string& operator=(const CharT* ps)&;

	~m_basic_string();

	m_basic_string& operator+=(const m_basic_string& s)&;

	Proxy operator[](size_t i);
	CharT operator[](size_t i) const;

	void check(size_t i) const;
	void write(size_t i, CharT c);
	CharT read(size_t i) const;

	void colorize(unsigned __int8, unsigned __int8, unsigned __int8);
	void make_italic();
	void make_bold();

	m_basic_string tolower() const;
	m_basic_string touppper() const;


	inline size_t len() const
	{
		return rep_->len();
	};

	inline const Color& color() const {
		return rep_->color();
	}

	inline const CharT* c_str() const
	{
		return rep_->str();
	};

	inline bool empty() const
	{
		return rep_->len() == 0;
	};

	explicit inline operator const char* () const
	{
		return c_str();
	};

	explicit inline operator
		std::basic_string<CharT>() const
	{
		return std::basic_string<CharT>(c_str());
	};

	inline const char* begin() const
	{
		return &this->c_str()[0];
	};

	inline const char* end() const
	{
		return &this->c_str()[len() + 1];
	};

private:
	string_rep* rep_;
};

class Color {
	template <typename CharT, typename Traits>
	friend class m_basic_string<CharT, Traits>::string_rep;
private:
	unsigned __int8 r_;
	unsigned __int8 g_;
	unsigned __int8 b_;
public:
	static constexpr char rgb_sequence[]{ 0x1B,'[','3','8',';','2',';','\0' };
	static constexpr char reset[]{ 0x1B,'[','0','m','\0' };

	inline size_t get_r() const {
		return r_;
	};
	inline size_t get_g() const {
		return g_;
	};
	inline size_t get_b() const {
		return b_;
	};

	Color(
		size_t,
		size_t,
		size_t);
	~Color() = default;
};

Color::Color(
	size_t r,
	size_t g,
	size_t b)
	: r_(r), g_(g), b_(b)
{};

template <typename CharT>
std::basic_ostream<CharT>&
operator<<(std::basic_ostream<CharT>& os, const m_basic_string<CharT>& str)
{ 
	Color color = str.color();
	
	return os 
		<< Color::rgb_sequence 
		<< std::to_string(color.get_r()) << ';' 
		<< std::to_string(color.get_g()) << ';' 
		<< std::to_string(color.get_b()) << 'm' 
		<< str.c_str() << Color::reset;
};

template <typename C>
m_basic_string<C> operator+(m_basic_string<C>& l, const m_basic_string<C>& r)
{
	return l += r;
};

template <typename C, typename TS>
const m_basic_string<C, TS>
operator+(const C* const l, const m_basic_string<C, TS>& r)
{
	return m_basic_string<C>(l, r.c_str());
};

template <typename C, typename TS>
const m_basic_string<C, TS> operator+(const C l, const m_basic_string<C, TS>& r)
{
	return m_basic_string<C>(l, r.c_str());
}

template <typename C>
m_basic_string<C> operator+(m_basic_string<C>& l, const C* const r)
{
	return m_basic_string<C>(l.c_str(), r);
};

template <typename C>
m_basic_string<C> operator+(m_basic_string<C>& l, const C r)
{
	return m_basic_string<C>(l.c_str(), r);
};

template <typename C, typename TS = std::char_traits<C>>
bool operator==(const m_basic_string<C>& l, const m_basic_string<C>& r)
{
	return (TS::compare(l.c_str(), r.c_str(), l.len() + 1) == 0);
};

template <typename C, typename TS = std::char_traits<C>>
bool operator==(const m_basic_string<C>& l, const C* r)
{
	return (TS::compare(l.c_str(), r, l.len() + 1) == 0);
};

template <typename C, typename TS>
bool operator==(const C* l, const m_basic_string<C, TS>& r)
{
	return (r == l);
};

template <typename C>
bool operator!=(const m_basic_string<C>& l, const m_basic_string<C>& r)
{
	return !(l == (r));
};

template <typename C, typename TS>
bool operator!=(const m_basic_string<C, TS>& l, const C* r)
{
	return !(l == (r));
}

template <typename C, typename TS>
bool operator!=(const C* l, const m_basic_string<C, TS>& r)
{
	return r != l;
}

template <typename C, typename TS = std::char_traits<C>>
bool operator<(const m_basic_string<C>& l, const m_basic_string<C>& r)
{
	return (TS::compare(l.c_str(), r.c_str(), l.len() + 1) < 0);
};

template <typename C, typename TS>
bool operator<(const C* l, const m_basic_string<C, TS>& r)
{
	return (TS::compare(l, r.c_str(), r.len() + 1) < 0);
}

template <typename C, typename TS>
bool operator<(const m_basic_string<C, TS>& l,const C* r)
{
	return (!(r < l) && !(r == l));
}

template <typename C>
bool operator>(const m_basic_string<C>& l, const m_basic_string<C>& r)
{
	return !(l < r);
};

template <typename C, typename TS>
bool operator>(const C* l, const m_basic_string<C, TS>& r)
{
	return !(l < r);
}

template <typename C, typename TS>
bool operator>(const m_basic_string<C, TS>& l, const C* r)
{
	return !(l < r);
}

template <typename C>
bool operator<=(const m_basic_string<C>& l, const m_basic_string<C>& r)
{
	return l < r || l == r;
};

template <typename C, typename TS>
bool operator<=(const C* l, const m_basic_string<C, TS>& r)
{
	return l < r || l == r;
}

template <typename C, typename TS>
bool operator<=(const m_basic_string<C, TS>& l, const C* r)
{
	return l < r || l == r;
}

template <typename C>
bool operator>=(const m_basic_string<C>& l, const m_basic_string<C>& r)
{
	return !(l < r) || l == r;
};

template <typename C, typename TS>
bool operator>=(const C* l, const m_basic_string<C, TS>& r)
{
	return !(l < r) || l == r;
}

template <typename C, typename TS>
bool operator>=(const m_basic_string<C, TS>& l, const C* r)
{
	return !(l < r) || l == r;
}

using m_string = m_basic_string<char>;
using wm_string = m_basic_string<wchar_t>;
using u16m_string = m_basic_string<char16_t>;
using u32m_string = m_basic_string<char32_t>;

/*
*	m_basic_string
*/

//concatenation constructor
template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
m_basic_string(const CharT* const l, const CharT* const r)
	: rep_(new string_rep())
{
	size_t l_len = Traits::length(l);
	size_t r_len = Traits::length(r);
	size_t total_len = l_len + r_len;

	CharT* res = new CharT[total_len + 1];
	Traits::copy(res, l, l_len);
	Traits::copy(res + l_len, r, r_len + 1);

	rep_->assign(total_len,res);
};

//concatenation constructor
template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
m_basic_string(const CharT* const l, const CharT r)
	: m_basic_string(l, (new CharT[]{r,'\0'}))
{};

//concatenation constructor
template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
m_basic_string(const CharT l, const CharT* const r)
	: m_basic_string((new CharT[]{l,'\0'}), r)
{};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
m_basic_string(void)
	: rep_(new string_rep())
{};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
m_basic_string(const CharT c)
	: rep_(new string_rep(c))
{};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
~m_basic_string()
{
	if (--rep_->use_count_ == 0)
		delete rep_;
};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
m_basic_string(const CharT* const s)
	: rep_(new string_rep(Traits::length(s), s))
{};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
m_basic_string(std::basic_string<CharT>& s)
	: m_basic_string(s.c_str())
{};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
m_basic_string(const m_basic_string& c)
	: m_basic_string()
{
	++c.rep_->use_count_;
	rep_ = c.rep_;
};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>&
m_basic_string<CharT, Traits>::
operator=(const m_basic_string& s)&
{
	++s.rep_->use_count_;

	if (--rep_->use_count_ == 0)
		delete rep_;

	rep_ = s.rep_;

	return *this;
};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>&
m_basic_string<CharT, Traits>::
operator=(const CharT* ps)&
{
	if (ps == 0)
		throw bad_pointer();
	if (rep_->use_count_ == 1)
		rep_->assign(Traits::length(ps), ps);
	else
	{
		rep_->use_count_--;
		rep_ = new string_rep(Traits::length(ps), ps);
	}
	return *this;
}

//operator +=
template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>&
m_basic_string<CharT, Traits>::
operator+=(const m_basic_string& s)&
{
	size_t curr_len = len();
	size_t add_len = s.len();
	size_t total_len = curr_len + add_len;

	CharT* s_res = new CharT[total_len + 1];
	Traits::copy(s_res, rep_->str_, curr_len);
	Traits::copy(s_res + curr_len, s.rep_->str_, add_len + 1);

	this->operator=(s_res);

	return *this;
};

//modifier[]
template <typename CharT, typename Traits>
typename m_basic_string<CharT, Traits>::Proxy
m_basic_string<CharT, Traits>::
operator[](size_t i)
{
	check(i);
	return Proxy(*this, i);
};

//selector[]
template <typename CharT, typename Traits>
CharT m_basic_string<CharT, Traits>::
operator[](size_t i) const 
{
	check(i);
	return rep_->str_[i];
};

template <typename CharT, typename Traits>
void m_basic_string<CharT, Traits>::
check(size_t i) const
{
	if (rep_->len_ <= i)
		throw bad_index();
}

template <typename CharT, typename Traits>
void m_basic_string<CharT, Traits>::
write(size_t i, CharT c)
{
	rep_ = rep_->get_own_copy();
	rep_->str_[i] = c;

	return;
}

template <typename CharT, typename Traits>
CharT m_basic_string<CharT, Traits>::
read(size_t i) const
{
	return rep_->str_[i];
}


template <typename CharT, typename Traits>
void m_basic_string<CharT, Traits>::
colorize(
	unsigned __int8 r,
	unsigned __int8 g,
	unsigned __int8 b) {

	rep_ = rep_->get_own_copy();
	rep_->colorize(r, g, b);
}

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>
m_basic_string<CharT, Traits>::
tolower() const
{
	size_t len = rep_->len();
	CharT* buffer = new CharT[len + 1];

	for (int i = 0; i < len + 1; i++)
		buffer[i] = std::tolower(rep_->str_[i]);

	return m_basic_string(buffer);
}

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>
m_basic_string<CharT, Traits>::
touppper() const
{
	size_t len = rep_->len();
	CharT* buffer = new CharT[len + 1];

	for (int i = 0; i < len + 1; i++)
		buffer[i] = std::toupper(rep_->str_[i]);

	return m_basic_string(buffer);
}

/*
*	string_rep
*/

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
string_rep::string_rep(void)
	: len_(0)
	, str_(new CharT[1])
	, use_count_(1)
	, color_(*(new Color(192, 192, 192)))
{
	*str_ = '\0';
};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
string_rep::string_rep(const CharT c)
	: len_(1)
	, str_(new CharT[2])
	, use_count_(1)
	, color_(*(new Color(192, 192, 192)))
{
	*str_ = c;
	*(str_ + 1) = '\0';
};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
string_rep::string_rep(size_t len, const CharT* s)
	: len_(len)
	, str_(new CharT[len_ + 1])
	, use_count_(1)
	, color_(*(new Color(192, 192, 192)))
{
	Traits::copy(str_, s, len_ + 1);
};

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::
string_rep::~string_rep()
{
	delete str_;
};

template <typename CharT, typename Traits>
void m_basic_string<CharT, Traits>::
string_rep::
colorize(unsigned __int8 r, unsigned __int8 g, unsigned __int8 b) {
	color_.r_ = r;
	color_.g_ = g;
	color_.b_ = b;
};

template <typename CharT, typename Traits>
typename m_basic_string<CharT, Traits>::string_rep*
m_basic_string<CharT, Traits>::string_rep::
get_own_copy()
{
	if (use_count_ == 1)
		return this;
	use_count_--;
	return new string_rep(len_, str_);
}

template <typename CharT, typename Traits>
void m_basic_string<CharT, Traits>::
string_rep::assign(size_t len, const CharT* ps)
{
	if (ps == 0)
		throw bad_pointer();
	if (len_ != len)
	{
		delete[] str_;
		len_ = len;
		str_ = new CharT[len_ + 1];
	}
	Traits::copy(str_, ps, len_ + 1);
	return;
}

/*
*	proxy
*/

template <typename CharT, typename Traits>
m_basic_string<CharT, Traits>::Proxy::
Proxy(m_basic_string& s, size_t pos)
	: b_str_(s)
	, pos_(pos)
{};

template <typename CharT, typename Traits>
inline m_basic_string<CharT, Traits>::Proxy::
operator CharT() const
{
	return b_str_.read(pos_);
};

template <typename CharT, typename Traits>
typename m_basic_string<CharT, Traits>::Proxy&
m_basic_string<CharT, Traits>::
Proxy::operator=(const CharT c)
{
	b_str_.write(pos_, c);
	return *this;
};





