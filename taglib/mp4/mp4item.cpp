/**************************************************************************
    copyright            : (C) 2007 by Lukáš Lalinský
    email                : lalinsky@gmail.com
 **************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>

#include <taglib.h>
#include <tdebug.h>
#include "mp4item.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1400)  // VC++2005 or later
# define SPRINTF sprintf_s
#else
# define SPRINTF sprintf
#endif

using namespace TagLib;

class MP4::Item::ItemPrivate : public RefCounter
{
public:
  ItemPrivate() : RefCounter(), valid(true), atomDataType(MP4::TypeUndefined), type(MP4::Item::TypeUndefined) {}

  bool valid;
  AtomDataType atomDataType;
  ItemType type;
  union {
    bool m_bool;
    int m_int;
    IntPair m_intPair;
    uchar m_byte;
    uint m_uint;
    long long m_longlong;
  };
  StringList m_stringList;
  ByteVectorList m_byteVectorList;
  MP4::CoverArtList m_coverArtList;
};

MP4::Item::Item()
{
  d = new ItemPrivate;
  d->valid = false;
}

MP4::Item::Item(const Item &item) : d(item.d)
{
  d->ref();
}

MP4::Item &
MP4::Item::operator=(const Item &item)
{
  if(d->deref()) {
    delete d;
  }
  d = item.d;
  d->ref();
  return *this;
}

MP4::Item::~Item()
{
  if(d->deref()) {
    delete d;
  }
}

MP4::Item::Item(bool value)
{
  d = new ItemPrivate;
  d->m_bool = value;
  d->type = TypeBool;
}

MP4::Item::Item(int value)
{
  d = new ItemPrivate;
  d->m_int = value;
  d->type = TypeInt;
}

MP4::Item::Item(uchar value)
{
  d = new ItemPrivate;
  d->m_byte = value;
  d->type = TypeByte;
}

MP4::Item::Item(uint value)
{
  d = new ItemPrivate;
  d->m_uint = value;
  d->type = TypeUInt;
}

MP4::Item::Item(long long value)
{
  d = new ItemPrivate;
  d->m_longlong = value;
  d->type = TypeLongLong;
}

MP4::Item::Item(int value1, int value2)
{
  d = new ItemPrivate;
  d->m_intPair.first = value1;
  d->m_intPair.second = value2;
  d->type = TypeIntPair;
}

MP4::Item::Item(const ByteVectorList &value)
{
  d = new ItemPrivate;
  d->m_byteVectorList = value;
  d->type = TypeByteVectorList;
}

MP4::Item::Item(const StringList &value)
{
  d = new ItemPrivate;
  d->m_stringList = value;
  d->type = TypeStringList;
}

MP4::Item::Item(const MP4::CoverArtList &value)
{
  d = new ItemPrivate;
  d->m_coverArtList = value;
  d->type = TypeCoverArtList;
}

void MP4::Item::setAtomDataType(MP4::AtomDataType type)
{
  d->atomDataType = type;
}

MP4::AtomDataType MP4::Item::atomDataType() const
{
  return d->atomDataType;
}

bool
MP4::Item::toBool() const
{
  return d->m_bool;
}

int
MP4::Item::toInt() const
{
  return d->m_int;
}

uchar
MP4::Item::toByte() const
{
  return d->m_byte;
}

TagLib::uint
MP4::Item::toUInt() const
{
  return d->m_uint;
}

long long
MP4::Item::toLongLong() const
{
  return d->m_longlong;
}

MP4::Item::IntPair
MP4::Item::toIntPair() const
{
  return d->m_intPair;
}

StringList
MP4::Item::toStringList() const
{
  return d->m_stringList;
}

ByteVectorList
MP4::Item::toByteVectorList() const
{
  return d->m_byteVectorList;
}

MP4::CoverArtList
MP4::Item::toCoverArtList() const
{
  return d->m_coverArtList;
}

bool
MP4::Item::isValid() const
{
  return d->valid;
}

String
MP4::Item::toString() const
{
  StringList desc;
  char tmp[256];
  switch (d->type) {
    case TypeBool:
      return d->m_bool ? "true" : "false";
    case TypeInt:
      SPRINTF(tmp, "%d", d->m_int);
      return tmp;
    case TypeIntPair:
      SPRINTF(tmp, "%d/%d", d->m_intPair.first, d->m_intPair.second);
      return tmp;
    case TypeByte:
      SPRINTF(tmp, "%d", d->m_byte);
      return tmp;
    case TypeUInt:
      SPRINTF(tmp, "%u", d->m_uint);
      return tmp;
    case TypeLongLong:
      SPRINTF(tmp, "%lld", d->m_longlong);
      return tmp;
    case TypeStringList:
      return d->m_stringList.toString(" / ");
    case TypeByteVectorList:
      for(TagLib::uint i = 0; i < d->m_byteVectorList.size(); i++) {
        SPRINTF(tmp, "[%d bytes of data]", d->m_byteVectorList[i].size());
        desc.append(tmp);
      }
      return desc.toString(", ");
    case TypeCoverArtList:
      for(TagLib::uint i = 0; i < d->m_coverArtList.size(); i++) {
        SPRINTF(tmp, "[%d bytes of data]", d->m_coverArtList[i].data().size());
        desc.append(tmp);
      }
      return desc.toString(", ");
    case TypeUndefined:
      return "[unknown]";
  }
  return String();
}

