/*
 * MenuCallback.h
 *
 *  Created on: Sep 29, 2016
 *      Author: sgaer
 */

#ifndef MENUCALLBACK_H_
#define MENUCALLBACK_H_

/**
 * @brief The MenuCallback class is an abstract base class that defines the interface for menu callbacks.
 */
class MenuCallback {
public:
	/**
  * @brief Constructs a MenuCallback object.
  */
	MenuCallback();

	/**
  * @brief Destroys the MenuCallback object.
  */
	virtual ~MenuCallback();

	/**
  * @brief This method is called when a menu choice is made.
  * @param iKey The key representing the selected menu choice.
  */
	virtual void onMenuChoice(int iKey) = 0;
};

#endif /* MENUCALLBACK_H_ */
