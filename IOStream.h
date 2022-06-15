#pragma once

// 本文件包含部分标准输入输出流相关类的前置申明

/********************** 
 * 除非必要，请勿修改 * 
 **********************/

namespace std {

template<class> class char_traits;
template<class, class> class basic_ostream;
template<class, class> class basic_istream;
typedef basic_ostream<char, char_traits<char>> ostream;
typedef basic_istream<char, char_traits<char>> istream;

}  // namespace std
