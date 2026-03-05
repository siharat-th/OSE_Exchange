/*
 * OnixStructs.hpp
 *
 *  Created on: Feb 1, 2021
 *      Author: sgaer
 */

 #ifndef KTNSTRUCTS_HPP_
 #define KTNSTRUCTS_HPP_
 
 #include <algorithm>
 #include <cstdarg>
 #include <fstream>
 #include <iomanip>
 #include <iostream>
 #include <iterator>
 #include <limits>
 #include <map>
 #include <math.h>
 #include <memory.h>
 #include <memory>
 #include <ostream>
 #include <sstream>
 #include <stdexcept>
 #include <stdio.h>
 #include <string>
 #include <vector>
 #include <cstddef>
 
 using namespace std;
 
 /**
  * @brief Struct representing a risk limit.
  */
 struct RiskLimit
 {
     string account; /**< The account associated with the risk limit. */
     string key; /**< The key of the risk limit. */
     double max_ord_notional; /**< The maximum order notional of the risk limit. */
 };
 
 /**
  * @brief Struct representing user information.
  */
 struct UserInfo
 {
     std::string user; /**< The username of the user. */
     std::string org; /**< The organization of the user. */
     std::string groupname; /**< The group name of the user. */

	/**
	 * @brief Static method to construct a UserInfo object.
	 * @param user The username of the user.
	 * @param org The organization of the user.
	 * @param groupname The group name of the user.
	 * @return The constructed UserInfo object.
	 */
	static UserInfo Construct(const std::string& user, const std::string& org, const std::string& groupname)
	{
		UserInfo u;
		u.user = user;
		u.org = org;
		u.groupname = groupname;
		return u;
	}

     /**
   * @brief Prints the user information.
   * @param u The user information to print.
   * @return The formatted string representation of the user information.
   */
     static std::string Print(UserInfo u)
     {
         ostringstream oss;
         oss << "user=" << u.user << " org=" << u.org << " groupname=" << u.groupname;
         return oss.str();
     }
 };
 
 #endif /* ONIXSTRUCTS_HPP_ */
 