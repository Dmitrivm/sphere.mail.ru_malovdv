#pragma once
#include <iostream>

enum class Error
{
    NoError,
    CorruptedArchive
};

class Serializer
{
    static constexpr char Separator = ' ';
public:

    std::ostream& out_;

    explicit Serializer(std::ostream& out)
        : out_(out)
    {
    }

    template <class T>
    Error save(T&& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args) {
        return process(std::forward<ArgsT>(args)...);
    }
    
private:
    // process использует variadic templates

    Error print(bool val) {
        std::string s("");
        if (val) 
            s = "true";
        else
            s = "false";

        if (out_ << s << Separator) 
            return Error::NoError;

        return Error::CorruptedArchive; 
    }

    Error print(uint64_t val) {
        if (out_ << val << Separator) 
            return Error::NoError;

        return Error::CorruptedArchive;       
    }

    template <class T>
    Error process(T&& val)
    {
        return print(std::forward<T>(val));
    }

    template <class T, class... ArgsT>
    Error process(T&& val, ArgsT&&... args) {
        if (process(std::forward<T>val) == Error::NoError)
            return process(std::forward<ArgsT>(args)...);

        return Error::CorruptedArchive;
    }


};

class Deserializer
{
public:
    explicit Deserializer(std::istream& in)
        : in_(in)
    {
    }

    template <class T>
    Error load(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args)
    {
        return process(std::forward<ArgsT>(args)...);
    }
    
private:
    std::istream& in_;
    // process использует variadic templates

    Error doLoad(bool& value)
    {
        std::string text;
        in_ >> text;

        if (text == "true")
            value = true;
        else if (text == "false")
            value = false;
        else
            return Error::CorruptedArchive;

        return Error::NoError;
    }

    Error doLoad(uint64_t& value)
    {
        std::string text;
        in_ >> text;

        try {
            value = std::stoi(text);
        }
        catch (std::logic_error& ex) {
            return Error::CorruptedArchive;
        }

        if (text[0] == '-') {
            return Error::CorruptedArchive;
        }
        return Error::NoError;
    }

    template <class T>
    Error process(T&& val)
    {
        return doLoad(std::forward<T>(val));
    }

    template <class T, class... ArgsT>
    Error process(T&& val, ArgsT&&... args) {
        if (doLoad(std::forward<T>(val)) == Error::NoError)
            return process(std::forward<ArgsT>(args)...);

        return Error::CorruptedArchive;
    }

};