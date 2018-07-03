#pragma once

#include "RefItem.h"
#include "ByteBuffer.h"

class CFile : public IFile
{
public:
	 CFile(cstring _fullFileName);
	 CFile(cstring _name, cstring _path);
	 virtual ~CFile();

	 // IRefItem
	 /*void AddRef() override { CRefItem::AddRef(); }
	 void Release() override { CRefItem::Release(); }
	 uint32 GetRefsCount() const { return CRefItem::GetRefsCount();	 }*/
	 void AddRef() override { m_RefsCount += 1; }
	 void Release() override { m_RefsCount -= 1; }
	 uint32 GetRefsCount() const override { return m_RefsCount; }

	 // IFile
	 string Name() const override { return m_Name; }
	 string Path() const override { return m_Path; }
	 string Extension() const override { return m_Extension; }
	 string Path_Name() const override { return string(m_Path + m_Name); }

	 // IByteBuffer
	 uint64_t getSize() const override 
	 { 
		 m_FilesManager->Guard();
		 return m_ByteBuffer.getSize(); 
	 }
	 uint64_t getPos() const override 
	 { 
		 m_FilesManager->Guard();
		 return m_ByteBuffer.getPos(); 
	 }
	 const uint8* getData() const override 
	 { 
		 m_FilesManager->Guard();
		 return m_ByteBuffer.getData(); 
	 }
	 const uint8* getDataFromCurrent() const override 
	 { 
		 m_FilesManager->Guard();
		 return m_ByteBuffer.getDataFromCurrent(); 
	 }
	 bool isEof() const override 
	 { 
		 m_FilesManager->Guard();
		 return m_ByteBuffer.isEof(); 
	 }
	 void seek(uint64_t _bufferOffsetAbsolute) override 
	 { 
		 m_FilesManager->Guard();
		 m_ByteBuffer.seek(_bufferOffsetAbsolute); 
	 }
	 void seekRelative(uint64_t _bufferOffsetRelative) override 
	 { 
		 m_FilesManager->Guard();
		 m_ByteBuffer.seekRelative(_bufferOffsetRelative); 
	 }
	 string readLine() override 
	 { 
		 m_FilesManager->Guard();
		 return m_ByteBuffer.readLine(); 
	 }
	 void readBytes(void* _destination, uint64_t _size) override 
	 { 
		 m_FilesManager->Guard();
		 m_ByteBuffer.readBytes(_destination, _size); 
	 }

public:
	static void FixFilePath(string& _string);

private:
	void ParsePathAndExtension();

protected:
	ByteBuffer m_ByteBuffer;
	IFilesManager* m_FilesManager;

private: // IFile
	string m_Name;
	string m_Path;
	string m_Extension;

private: // IRefItem
	uint32 m_RefsCount;
};