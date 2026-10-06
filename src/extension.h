/**
 * BotTest — главный класс расширения.
 */

#ifndef _INCLUDE_BOTTEST_EXTENSION_H_
#define _INCLUDE_BOTTEST_EXTENSION_H_

#include "smsdk_ext.h"

class BotTest : public SDKExtension
{
public:
	/**
	 * @brief Вызывается после первичной загрузки расширения.
	 *
	 * @param error		Буфер для сообщения об ошибке.
	 * @param maxlength	Размер буфера.
	 * @param late		Загружено ли расширение после загрузки карты.
	 * @return			True — успех, false — неудача.
	 */
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);

	/**
	 * @brief Вызывается, когда все расширения загружены.
	 * Хорошее место для регистрации нативов.
	 */
	virtual void SDK_OnAllLoaded();
};

#endif // _INCLUDE_BOTTEST_EXTENSION_H_
