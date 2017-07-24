/*
 * GEL++ File interface
 * Copyright (c) 2016, IRIT- université de Toulouse
 *
 * GEL++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GEL++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GEL++; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef GELPP_FILE_H_
#define GELPP_FILE_H_

#include <elm/data/Array.h>
#include <elm/sys/Path.h>
#include <gel++/base.h>

namespace gel {

using namespace elm;
class Image;
class Manager;
namespace elf { class File; }

class Segment {
public:
	virtual ~Segment(void);
	virtual cstring name(void) throw(Exception) = 0;
	virtual address_t baseAddress(void) = 0;
	virtual address_t loadAddress(void) = 0;
	virtual size_t size(void) = 0;
	virtual size_t alignment(void) = 0;
	virtual bool isExecutable(void) = 0;
	virtual bool isWritable(void) = 0;
	virtual bool hasContent(void) = 0;
	virtual Buffer buffer(void) throw(Exception) = 0;
};

class File {
public:
	typedef enum {
		no_type,
		program,
		library
	} type_t;

	File(Manager& manager, sys::Path path);
	virtual ~File(void);

	inline sys::Path path(void) const { return _path; }
	inline io::IntFormat format(address_t a) { return gel::format(addressType(), a); }

	virtual elf::File *toELF(void);

	virtual type_t type(void) = 0;
	virtual bool isBigEndian(void) = 0;
	virtual address_type_t addressType(void) = 0;
	virtual address_t entry(void) = 0;
	virtual int count(void) const = 0;
	virtual Segment *segment(int i) const = 0;
	virtual Image *make(void) throw(Exception) = 0;

protected:
	Manager& man;
private:
	sys::Path _path;
};

io::Output& operator<<(io::Output& out, File::type_t t);

} // gel

#endif /* GELPP_FILE_H_ */
