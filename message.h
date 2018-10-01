#pragma once

#include <stdint.h>
#include <string>

class PkgFormat
{
public:
	PkgFormat() =default;
	~PkgFormat() = default;

	static uint32_t EncodeFloat(float value);
	static float DecodeFloat(uint32_t value);
	static uint64_t EncodeDouble(double value);
	static double DecodeDouble(uint64_t value);
	
	static uint32_t ZigZagEncode32(int32_t n);
	static int32_t ZigZagDecode32(uint32_t n);
	static uint64_t ZigZagEncode64(int64_t n);
	static int64_t ZigZagDecode64(uint64_t n);
	
	static uint32_t ReadVarint32(uint8_t* buffer, uint32_t* value);
	static uint32_t WriteVarInt32(uint32_t value, uint8_t* buffer);
	static uint32_t ReadVarint64(uint8_t* buffer, uint64_t* value);	
	static uint32_t WriteVarInt64(uint64_t value, uint8_t* buffer);

	static uint32_t TagSize(uint32_t number);
	static uint32_t ReadTag(uint8_t* buffer, uint32_t* number);
	static uint32_t WriteTag(uint32_t number, uint8_t* buffer);
	 
	static uint32_t Int32Size(uint32_t number, int32_t value);
	static uint32_t ReadInt32NoTag(uint8_t* buffer, int32_t* value);
	static uint32_t WriteInt32NoTag(int32_t value, uint8_t* buffer);
	static uint32_t WriteInt32(uint32_t number, int32_t value, uint8_t* buffer);
	 
	static uint32_t Int64Size(uint32_t number, int64_t value);
	static uint32_t ReadInt64NoTag(uint8_t* buffer, int64_t* value);
	static uint32_t WriteInt64NoTag(int64_t value, uint8_t* buffer);
	static uint32_t WriteInt64(uint32_t number, int64_t value, uint8_t* buffer);
	 
	static uint32_t UInt32Size(uint32_t number, uint32_t value);
	static uint32_t ReadUInt32NoTag(uint8_t* buffer, uint32_t* value);
	static uint32_t WriteUInt32NoTag(uint32_t value, uint8_t* buffer);
	static uint32_t WriteUInt32(uint32_t number, uint32_t value, uint8_t* buffer);
	 
	static uint32_t UInt64Size(uint32_t number, uint64_t value);
	static uint32_t ReadUInt64NoTag(uint8_t* buffer, uint64_t* value);
	static uint32_t WriteUInt64NoTag(uint64_t value, uint8_t* buffer);
	static uint32_t WriteUInt64(uint32_t number, uint64_t value, uint8_t* buffer);
	 
	static uint32_t DoubleSize(uint32_t number, double value);
	static uint32_t ReadDoubleNoTag(uint8_t* buffer, double* value);
	static uint32_t WriteDoubleNoTag(double value, uint8_t* buffer);
	static uint32_t WriteDouble(uint32_t number, double value, uint8_t* buffer);
	 
	static uint32_t FloatSize(uint32_t number, float value);
	static uint32_t ReadFloatNoTag(uint8_t* buffer, float* value);
	static uint32_t WriteFloatNoTag(float value, uint8_t* buffer);
	static uint32_t WriteFloat(uint32_t number, float value, uint8_t* buffer);
	 
	static uint32_t ByteSize(uint32_t number, uint8_t value);
	static uint32_t ReadByteNoTag(uint8_t* buffer, uint8_t* value);
	static uint32_t WriteByteNoTag(uint8_t value, uint8_t* buffer);
	static uint32_t WriteByte(uint32_t number, uint8_t value, uint8_t* buffer);

	static uint32_t BoolSize(uint32_t number, bool value);
	static uint32_t ReadBoolNoTag(uint8_t* buffer, bool* value);
	static uint32_t WriteBoolNoTag(bool value, uint8_t* buffer);
	static uint32_t WriteBool(uint32_t number, bool value, uint8_t* buffer);
	 
	static uint32_t StringSize(uint32_t number, const std::string& value);
	static uint32_t ReadStringNoTag(uint8_t* buffer, std::string* value);
	static uint32_t WriteStringNoTag(const std::string& value, uint8_t* buffer);
	static uint32_t WriteString(uint32_t number, const std::string& value, uint8_t* buffer);
};



inline uint32_t PkgFormat::EncodeFloat(float value) {
	union { float f; uint32_t i; };
	f = value;
	return i;
}

inline float PkgFormat::DecodeFloat(uint32_t value) {
	union { float f; uint32_t i; };
	i = value;
	return f;
}

inline uint64_t PkgFormat::EncodeDouble(double value) {
	union { double f; uint64_t i; };
	f = value;
	return i;
}

inline double PkgFormat::DecodeDouble(uint64_t value) {
	union { double f; uint64_t i; };
	i = value;
	return f;
}

inline uint32_t PkgFormat::ZigZagEncode32(int32_t n) {
	return (static_cast<uint32_t>(n) << 1) ^ static_cast<uint32_t>(n >> 31);
}

inline int32_t PkgFormat::ZigZagDecode32(uint32_t n) {
	return static_cast<int32_t>((n >> 1) ^ (~(n & 1) + 1));
}

inline uint64_t PkgFormat::ZigZagEncode64(int64_t n) {
	return (static_cast<uint64_t>(n) << 1) ^ static_cast<uint64_t>(n >> 63);
}

inline int64_t PkgFormat::ZigZagDecode64(uint64_t n) {
	return static_cast<int64_t>((n >> 1) ^ (~(n & 1) + 1));
}

inline uint32_t PkgFormat::ReadVarint32(uint8_t* buffer, uint32_t* value) {
	uint32_t size = 0;
	const uint8_t* ptr = buffer;
	uint32_t result = 0;
	uint32_t b;

	b = *(ptr++); result = (b & 0x7F); if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 7; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= b << 28; if (!(b & 0x80)) goto done;

done:
	size = (uint32_t)(ptr - buffer);
	*value = result;
	return size;
}

inline uint32_t PkgFormat::WriteVarInt32(uint32_t value, uint8_t* buffer) {
	uint8_t* target = (uint8_t*)buffer;

	while (value >= 0x80) {
		*target = static_cast<uint8_t>(value | 0x80);
		value >>= 7;
		++target;
	}

	*target = static_cast<uint8_t>(value);
	++target;

	return uint32_t(target - buffer);
}



inline uint32_t PkgFormat::ReadVarint64(uint8_t* buffer, uint64_t* value) {
	uint32_t size = 0;
	const uint8_t* ptr = buffer;
	uint32_t part0 = 0, part1 = 0, part2 = 0;
	uint64_t b;

	b = *(ptr++); part0 = (b & 0x7F); if (!(b & 0x80)) goto done;
	b = *(ptr++); part0 |= (b & 0x7F) << 7; if (!(b & 0x80)) goto done;
	b = *(ptr++); part0 |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
	b = *(ptr++); part0 |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;

	b = *(ptr++); part1 = (b & 0x7F); if (!(b & 0x80)) goto done;
	b = *(ptr++); part1 |= (b & 0x7F) << 7; if (!(b & 0x80)) goto done;
	b = *(ptr++); part1 |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
	b = *(ptr++); part1 |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;

	b = *(ptr++); part2 = (b & 0x7F); if (!(b & 0x80)) goto done;
	b = *(ptr++); part2 |= (b & 0x7F) << 7; if (!(b & 0x80)) goto done;;

done:
	size = (uint32_t)(ptr - buffer);
	*value = (static_cast<uint64_t>(part0)) | (static_cast<uint64_t>(part1) << 28) | (static_cast<uint64_t>(part2) << 56);
	return size;
}

inline uint32_t PkgFormat::WriteVarInt64(uint64_t value, uint8_t* buffer) {
	uint8_t* target = (uint8_t*)buffer;

	while (value >= 0x80) {
		*target = static_cast<uint8_t>(value | 0x80);
		value >>= 7;
		++target;
	}

	*target = static_cast<uint8_t>(value);
	++target;

	return uint32_t(target - buffer);
}

inline uint32_t PkgFormat::TagSize(uint32_t number) {
	uint32_t nums = 1;
	uint32_t x = number;
	while (x >= 0x80) {
		x >>= 7;
		++nums;
	}
	return nums;
}

inline uint32_t PkgFormat::ReadTag(uint8_t* buffer, uint32_t* number) {
	uint32_t value = 0;
	uint32_t offset = 0;
	offset = ReadVarint32(buffer, &value);
	*number = value;
	return offset;
}

inline uint32_t PkgFormat::WriteTag(uint32_t number, uint8_t* buffer) {
	return WriteVarInt32(number, buffer);
}

inline uint32_t PkgFormat::Int32Size(uint32_t number, int32_t value) {
	uint32_t nums = TagSize(number) + 1;

	uint32_t x = ZigZagEncode32(value);
	while (x >= 0x80) {
		x >>= 7;
		++nums;
	}
	return nums;
}

inline uint32_t PkgFormat::ReadInt32NoTag(uint8_t* buffer, int32_t* value) {
	uint8_t* input = buffer;
	uint32_t zagcode = 0;
	uint32_t offset = 0;
	offset = ReadVarint32(input, &zagcode); input += offset;
	*value = ZigZagDecode32(zagcode);
	return (uint32_t)(input - buffer);
}

inline uint32_t PkgFormat::WriteInt32NoTag(int32_t value, uint8_t* buffer) {
	uint32_t data = ZigZagEncode32(value);
	return WriteVarInt32(data, buffer);
}

inline uint32_t PkgFormat::WriteInt32(uint32_t number, int32_t value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteInt32NoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}

inline uint32_t PkgFormat::Int64Size(uint32_t number, int64_t value) {
	uint32_t nums = TagSize(number) + 1;

	uint64_t x = ZigZagEncode64(value);
	while (x >= 0x80) {
		x >>= 7;
		++nums;
	}
	return nums;
}

inline uint32_t PkgFormat::ReadInt64NoTag(uint8_t* buffer, int64_t* value) {
	uint8_t* input = buffer;
	uint64_t zagcode = 0;
	uint32_t offset = 0;
	offset = ReadVarint64(input, &zagcode); input += offset;
	*value = ZigZagDecode64(zagcode);
	return (uint32_t)(input - buffer);
}

inline uint32_t PkgFormat::WriteInt64NoTag(int64_t value, uint8_t* buffer) {
	uint64_t data = ZigZagEncode64(value);
	return WriteVarInt64(data, buffer);
}

inline uint32_t PkgFormat::WriteInt64(uint32_t number, int64_t value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteInt64NoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}

inline uint32_t PkgFormat::UInt32Size(uint32_t number, uint32_t value) {
	uint32_t nums = TagSize(number) + 1;

	uint32_t x = value;
	while (x >= 0x80) {
		x >>= 7;
		++nums;
	}
	return nums;
}

inline uint32_t PkgFormat::ReadUInt32NoTag(uint8_t* buffer, uint32_t* value) {
	uint8_t* input = buffer;
	uint32_t data = 0;
	uint32_t offset = 0;
	offset = ReadVarint32(input, &data); input += offset;
	*value = data;
	return (uint32_t)(input - buffer);
}

inline uint32_t PkgFormat::WriteUInt32NoTag(uint32_t value, uint8_t* buffer) {
	return WriteVarInt32(value, buffer);
}

inline uint32_t PkgFormat::WriteUInt32(uint32_t number, uint32_t value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteUInt32NoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}


inline uint32_t PkgFormat::UInt64Size(uint32_t number, uint64_t value) {
	uint32_t nums = TagSize(number) + 1;

	uint64_t x = value;
	while (x >= 0x80) {
		x >>= 7;
		++nums;
	}
	return nums;
}

inline uint32_t PkgFormat::ReadUInt64NoTag(uint8_t* buffer, uint64_t* value) {
	uint8_t* input = buffer;
	uint64_t data = 0;
	uint32_t offset = 0;
	offset = ReadVarint64(input, &data); input += offset;
	*value = data;
	return (uint32_t)(input - buffer);
}

inline uint32_t PkgFormat::WriteUInt64NoTag(uint64_t value, uint8_t* buffer) {
	return WriteVarInt64(value, buffer);
}

inline uint32_t PkgFormat::WriteUInt64(uint32_t number, uint64_t value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteUInt64NoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}


inline uint32_t PkgFormat::DoubleSize(uint32_t number, double value) {
	uint32_t nums = TagSize(number) + 1;
	uint64_t x = EncodeDouble(value);
	while (x >= 0x80) {
		x >>= 7;
		++nums;
	}
	return nums;
}

inline uint32_t PkgFormat::ReadDoubleNoTag(uint8_t* buffer, double* value) {
	uint8_t* input = buffer;
	uint64_t data = 0;
	uint32_t offset = 0;
	offset = ReadVarint64(input, &data); input += offset;
	*value = DecodeDouble(data);
	return (uint32_t)(input - buffer);
}

inline uint32_t PkgFormat::WriteDoubleNoTag(double value, uint8_t* buffer) {
	uint64_t data = EncodeDouble(value);
	return WriteVarInt64(data, buffer);
}

inline uint32_t PkgFormat::WriteDouble(uint32_t number, double value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteDoubleNoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}

inline uint32_t PkgFormat::FloatSize(uint32_t number, float value) {
	uint32_t nums = TagSize(number) + 1;
	uint64_t x = EncodeFloat(value);
	while (x >= 0x80) {
		x >>= 7;
		++nums;
	}
	return nums;
}


inline uint32_t PkgFormat::ReadFloatNoTag(uint8_t* buffer, float* value) {
	uint8_t* input = buffer;
	uint32_t data = 0;
	uint32_t offset = 0;
	offset = ReadVarint32(input, &data); input += offset;
	*value = DecodeFloat(data);
	return (uint32_t)(input - buffer);
}
inline uint32_t PkgFormat::WriteFloatNoTag(float value, uint8_t* buffer) {
	uint32_t data = EncodeFloat(value);
	return WriteVarInt32(data, buffer);
}

inline uint32_t PkgFormat::WriteFloat(uint32_t number, float value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteFloatNoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}

inline uint32_t PkgFormat::ByteSize(uint32_t number, uint8_t value) {
	uint32_t nums = TagSize(number);
	return nums + 1;
}

inline uint32_t PkgFormat::ReadByteNoTag(uint8_t* buffer, uint8_t* value) {
	*value = *buffer;
	return 1;
}

inline uint32_t PkgFormat::WriteByteNoTag(uint8_t value, uint8_t* buffer) {
	*buffer = value;
	return 1;
}

inline uint32_t PkgFormat::WriteByte(uint32_t number, uint8_t value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteByteNoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}

inline uint32_t PkgFormat::BoolSize(uint32_t number, bool value) {
	uint32_t nums = TagSize(number);
	return nums + 1;
}

inline uint32_t PkgFormat::ReadBoolNoTag(uint8_t* buffer, bool* value) {
	*value = *buffer;
	return 1;
}

inline uint32_t PkgFormat::WriteBoolNoTag(bool value, uint8_t* buffer) {
	*buffer = value ? 1 : 0;
	return 1;
}

inline uint32_t PkgFormat::WriteBool(uint32_t number, bool value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteBoolNoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}

inline uint32_t PkgFormat::StringSize(uint32_t number, const std::string& value) {
	uint32_t nums = TagSize(number) + 1;
	uint32_t x = uint32_t(value.size());
	nums += x;
	while (x >= 0x80) {
		x >>= 7;
		++nums;
	}

	return nums;
}

inline uint32_t PkgFormat::ReadStringNoTag(uint8_t* buffer, std::string* value) {
	uint32_t offset = 0;
	uint8_t* input = buffer;
	uint32_t length = 0;
	offset = ReadVarint32(input, &length); input += offset;

	(*value).assign((const char*)input, length);

	return offset + length;
}

inline uint32_t PkgFormat::WriteStringNoTag(const std::string& value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	uint32_t size = uint32_t(value.size());
	offset = WriteVarInt32(size, buffer); target += offset;
	memcpy(target, value.c_str(), size); target += size;

	return uint32_t(target - buffer);
}

inline uint32_t PkgFormat::WriteString(uint32_t number, const std::string& value, uint8_t* buffer) {
	uint8_t* target = buffer;
	uint32_t offset = 0;
	offset = WriteTag(number, target); target += offset;
	offset = WriteStringNoTag(value, target); target += offset;

	return uint32_t(target - buffer);
}

class Message
{
public:
	virtual uint32_t ByteSize() = 0;
	virtual void SerializeToArray(uint8_t* data, uint32_t size) = 0;
	virtual uint32_t ParseFromArray(uint8_t* data) = 0;
	Message() { buffer_ = NULL; }
	virtual ~Message() { if (buffer_) { free(buffer_); } }
protected:
	void* buffer_;
};

#define BASIC_INTERFACE 		public: virtual uint32_t ByteSize(); public: virtual void SerializeToArray(uint8_t* data, uint32_t size);		public: virtual uint32_t ParseFromArray(uint8_t* data); 
#define PKG_FIELD(type, name) 	public: inline void set_##name(type a){name##_ = a; has_##name##_ = true;} inline void ignore_##name(){has_##name##_ = false;} inline type name(){return name##_;} 	private: type name##_; bool has_##name##_{false};

#define PKG_INT32_FIELD(name) PKG_FIELD(int32_t, name)
#define PKG_INT64_FIELD(name) PKG_FIELD(int64_t, name)
#define PKG_UINT32_FIELD(name) PKG_FIELD(uint32_t, name)
#define PKG_UINT64_FIELD(name) PKG_FIELD(uint64_t, name)
#define PKG_BYTE_FIELD(name) PKG_FIELD(uint8_t, name)
#define PKG_BOOL_FIELD(name) PKG_FIELD(bool, name)
#define PKG_DOUBLE_FIELD(name) PKG_FIELD(double, name)
#define PKG_FLOAT_FIELD(name) PKG_FIELD(float, name)
#define PKG_STRING_FIELD(name, maxsize) public: inline void set_##name(const std::string& a){ size_t size = a.size(); if (size > maxsize) { size = maxsize; } name##_.assign(a, 0, size);  has_##name##_ = true;}  inline void ignore_##name(){has_##name##_ = false;} inline std::string name(){return name##_;} private: std::string name##_; bool has_##name##_{false};

