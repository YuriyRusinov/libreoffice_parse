/* @brief
 * Декларации типов для связывания uno и Qt
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */

#pragma once

#include <com/sun/star/text/XTextTable.hpp>

using namespace com::sun::star::text;
using namespace com::sun::star::uno;

Q_DECLARE_METATYPE(Reference< XTextTable >);
