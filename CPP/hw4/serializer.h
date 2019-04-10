#pragma once
#include <iostream>
#include <istream>
#include <cstdlib>
#include <sstream>

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
    Error operator()(ArgsT... args)
    {
        return process(args...);
    }
    
private:
    // process использует variadic templates

    void print(bool& val) {
        std::string s("");
        if (val) 
            s = "true";
        else
            s = "false";
        out_ << s << Separator;
    }

    void print(uint64_t& val) {
        out_ << val << Separator;        
    }

    template <class T>
    void process(T& val)
    {
        print(val);
    }

    template <class T, class... Args>
    Error process(T&& val, Args&&... args)
    {
        print(val);
        process(std::forward<Args>(args)...);
        return Error::NoError;
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
    Error load(T&& object)
    {

        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT... args)
    {
        return process(args...);
    }
    
private:
    std::istream& in_;
    // process использует variadic templates
    Error load(bool& value)
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

    Error load(uint64_t& value)
    {
        std::string text;
        in_ >> text;
        std::cout << text << std::endl;

        try {
            value = std::stoi(text);
            std::cout << value << std::endl;
        }
        catch (std::invalid_argument& ex) {
            return Error::CorruptedArchive;
        }
        catch (std::out_of_range& ex)
        {
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
        return load(val);
    }

    template <class T, class... Args>
    Error process(T&& val, Args&&... args)
    {
        Error err = load(val);
        if (err != Error::NoError)
            return Error::CorruptedArchive; 
        return process(std::forward<Args>(args)...);
    }
};