#pragma once


// any_cast를 위해
class IStructContainer {
public:
    virtual ~IStructContainer() = default;
}; // IStructContainer


// 실제 데이터를 들고 있는 템플릿 클래스
template <typename T>
class StructContainer : public IStructContainer {
public:
    StructContainer()
        : m_Data()
    {
    }
    StructContainer(const T& data)
        : m_Data(data)
    {
    }

    T& Get() { return m_Data; }
    void Set(const T& data) { m_Data = data; }

private:
    T m_Data;
}; // StructContainer