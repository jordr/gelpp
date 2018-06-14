/*
 * GEL++ Image class interface
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
#ifndef GELPP_IMAGE_H_
#define GELPP_IMAGE_H_

#include <elm/data/BiDiList.h>
#include <gel++/base.h>
#include <gel++/File.h>

namespace gel {

using namespace elm;
class Image;

class ImageSegment {
public:
	typedef t::uint32 flags_t;
	static const flags_t
		WRITABLE	= 0x01,
		EXECUTABLE	= 0x02,
		TO_FREE		= 0x04;

	ImageSegment(Buffer buf, address_t addr, flags_t flags, cstring name = "");
	ImageSegment(File *file, Segment *segment, address_t addr, cstring name = "");
	~ImageSegment(void);
	void clean(void);
	inline cstring name(void) const { return _name; }
	inline File *file(void) const { return _file; }
	inline Segment *segment(void) const { return _seg; }
	inline address_t base(void) const { return _base; }
	inline size_t size(void) const { return _buf.size(); }
	inline const Buffer& buffer(void) const { return _buf; }
	inline Buffer& buffer(void) { return _buf; }
	inline range_t range(void) const { return range_t(_base, _buf.size()); }
	inline flags_t flags(void) const { return _flags; }
	inline bool isWritable(void) const { return _flags & WRITABLE; }
	inline bool isExecutable(void) const { return _flags & EXECUTABLE; }

private:
	static cstring defaultName(flags_t flags);

	cstring _name;
	File *_file;
	Segment *_seg;
	address_t _base;
	Buffer _buf;
	flags_t _flags;
};

class Image {
public:
	typedef struct link_t {
		inline link_t(File *f, address_t b): file(f), base(b) { }
		File *file;
		address_t base;
	} link_t;

	Image(File *program);
	~Image(void);
	inline File *program(void) const { return _prog; }
	void clean(void);

	typedef BiDiList<link_t>::Iter LinkIter;
	inline LinkIter files(void) const { return LinkIter(_links); }

	typedef BiDiList<ImageSegment *>::Iter SegIter;
	inline SegIter segments(void) const { return SegIter(segs); }

	void add(File *file, address_t base = 0);
	void add(ImageSegment *segment);
	ImageSegment *at(address_t address);

private:
	File *_prog;
	BiDiList<link_t> _links;
	BiDiList<ImageSegment *> segs;
};

class Parameter {
public:
	static const cstring gen_abi, unix_abi;
	static const Parameter null;

	Parameter(void);
	virtual ~Parameter(void);
	virtual cstring abi(void) const;

	Array<cstring> arg;
	Array<cstring> env;
	bool stack_alloc, stack_at;
	address_t stack_addr;
	t::size stack_size;
	Array<sys::Path> paths;
	address_t *sp;
	ImageSegment **sp_segment;
};

class ImageBuilder {
public:
	ImageBuilder(File *file, const Parameter& params = Parameter::null) throw(gel::Exception);
	virtual ~ImageBuilder(void);
	virtual Image *build(void) throw(gel::Exception) = 0;
	virtual File *retrieve(string name) throw(gel::Exception) = 0;
protected:
	File *_prog;
	const Parameter& _params;
};

class SimpleBuilder: public ImageBuilder {
public:
	SimpleBuilder(File *file, const Parameter& params = Parameter::null) throw(gel::Exception);
	virtual Image *build(void) throw(gel::Exception);
	virtual File *retrieve(string name) throw(gel::Exception);
};

} // gel

#endif /* GELPP_IMAGE_H_ */
