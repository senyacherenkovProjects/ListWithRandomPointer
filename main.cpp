#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <ctime>
#include <assert.h>
#include <fstream>
#include <math.h>
#include <iostream>
#include <cstdio>
#include <map>

#define BOOST_TEST_MODULE test_main

#include <boost/test/included/unit_test.hpp>

constexpr int BIT_SIZE = sizeof (int)*8;
constexpr long long LAST_BYTE = 0x000000FF;

std::vector<int> printBinaryView(int n)
{
    std::vector<int> result;
    result.reserve(BIT_SIZE);

    while(n >= 2 || n <= -2)
    {
        int bit = n % 2;
        result.push_back(bit);
        n /= 2;
        if(n == 1 || n == -1)
            result.push_back(std::abs(n));
    }

    if(result.size() < BIT_SIZE)
        result.resize(BIT_SIZE, 0);

    if(n < 0)
    {
        for(auto& bit: result)
            bit == 0? bit = 1: bit = 0;

        if(result[0])
        {
            result[0] = 0;
            for(auto it = result.begin() + 1; it < result.end(); it++)
            {
                if(*it)
                {
                    *it = 0;
                    continue;
                }
                else
                {
                    *it = 1;
                    break;
                }
            }
        }
        else
            result[0] = 1;

        result[BIT_SIZE - 1] = 1;
    }

    for(auto it = result.rbegin(); it < result.rend(); it++)
        std::cout << *it;
    std::cout << std::endl;

    return result;
}

void removeDups(char* str)
{
    int i = 0;
    int j = 0;
    char current = str[i];
    while(current != '\0')
    {
        if(str[i + 1] != current)
        {
            current = str[i + 1];
            str[j + 1] = str[i + 1];
            j++;
        }
        i++;
    }
}

// структуру ListNode модифицровать нельзя
struct ListNode {
    ListNode * prev{nullptr};
    ListNode * next{nullptr};
    ListNode * rand{nullptr}; // указатель на произвольный элемент данного списка, либо
    std::string data;
};

class List {
public:
    List(ListNode* h = nullptr, int n = 0):
        head(h),
        count(n)
    {}

    void Serialize (FILE * file) // сохранение в файл (файл открыт с помощью fopen(path, "wb"))
    {
        if(file == nullptr) {
            perror("error during opening.txt");
            return;
        }        
        fputc(count, file);
        char bytes[8];

        //All we need to know is next, rand and data
        for (ListNode* cur = head; cur != nullptr; cur = cur->next)
        {
            //current node
            fputc(static_cast<int>(sizeof(ListNode*)), file);
            long long temp = reinterpret_cast<long long>(cur);
            longToBuff(temp, bytes);
            fwrite(bytes, static_cast<int>(sizeof (ListNode*)), 1, file);

            //next node
            if(cur->next)
            {
                fputc(static_cast<int>(sizeof(ListNode*)), file);
                temp = reinterpret_cast<long long>(cur->next);
                longToBuff(temp, bytes);
                fwrite(bytes, static_cast<int>(sizeof (ListNode*)), 1, file);
            }
            else
            {
                fputc(0, file);
                fputc(-1, file);
            }

            //rand node
            fputc(static_cast<int>(sizeof(ListNode*)), file);
            temp = reinterpret_cast<long long>(cur->rand);
            longToBuff(temp, bytes);
            fwrite(bytes, static_cast<int>(sizeof (ListNode*)), 1, file);

            //data
            fputc(static_cast<int>(cur->data.size()), file);
            fwrite(cur->data.data(), cur->data.size(), 1, file);
        }
    }
    void Deserialize (FILE * file) // загрузка из файла (файл открыт с помощью fopen(path, "rb"))
    {
        count = fgetc(file);
        std::vector<std::pair<ListNode*, ListNode>> tempData;
        char pointer[sizeof(ListNode*)];
        ListNode* key = nullptr;
        for(int i = 0; i < count; i++)
        {
            ListNode node;

            int pSize = fgetc(file);
            if(pSize == sizeof(ListNode*))
            {
                char* number = fgets(pointer, sizeof(ListNode*),file);
                if(number != nullptr)
                    key = reinterpret_cast<ListNode*>(BuffTolong(pointer));
                else
                    std::cout << "node addr with error" << std::endl;
            }
            else
                key = nullptr;

            pSize = fgetc(file);
            pSize = fgetc(file);
            if(pSize == sizeof(ListNode*))
            {
                fgets(pointer, sizeof(ListNode*),file);
                node.next = reinterpret_cast<ListNode*>(BuffTolong(pointer));
            }
            else
                node.next = nullptr;

            pSize = fgetc(file);
            pSize = fgetc(file);
            if(pSize == sizeof(ListNode*))
            {
                fgets(pointer, sizeof(ListNode*),file);
                node.rand = reinterpret_cast<ListNode*>(BuffTolong(pointer));
            }
            else
                node.rand = nullptr;

            pSize = fgetc(file);
            pSize = fgetc(file);
            char* data = new char[static_cast<size_t>(pSize)];

            fgets(data, pSize + 1,file);
            node.data = std::string(data, static_cast<size_t>(pSize));

            tempData.emplace_back(std::make_pair(key, node));
        }

        for(auto& node: tempData)
        {
            ListNode* realNode = new ListNode;
            realNode->data = node.second.data;
            realNode->next = node.second.rand;
            node.second.rand = realNode;
        }

        ListNode* result = tempData[0].second.rand;
        for(auto& node: tempData)
        {
            ListNode* dup = node.second.rand;
            auto res = std::find_if(tempData.begin(), tempData.end(), [dup](auto& pair)
                {
                    if(pair.first == dup->next)
                        return true;
                    return false;
                });
            if(res != tempData.end())
                dup->rand = res->second.rand;
        }

        ListNode* prev = nullptr;
        for(auto& node: tempData)
        {
            ListNode* dup = node.second.rand;

            if(node.second.next)
            {
                auto res = std::find_if(tempData.begin(), tempData.end(), [node](auto& pair)
                    {
                        if(pair.first == node.second.next)
                            return true;
                        return false;
                    });
                if(res != tempData.end())
                    dup->next = res->second.rand;
            }
            else
                dup->next = nullptr;
            if(!dup->next)
                tail = dup;

            dup->prev = prev;
            prev = dup;
        }
        head = result;
    }

    ListNode* getHead() const {return head;}
    ListNode* getTail() const {return tail;}
    int getCount() const {return count;}
private:
    void longToBuff(long long data, char* output)
    {
        output[0] = static_cast<char>(data & LAST_BYTE);
        output[1] = static_cast<char>(data >> 8 & LAST_BYTE);
        output[2] = static_cast<char>(data >> 16 & LAST_BYTE);
        output[3] = static_cast<char>(data >> 24 & LAST_BYTE);
        output[4] = static_cast<char>(data >> 32 & LAST_BYTE);
        output[5] = static_cast<char>(data >> 40 & LAST_BYTE);
        output[6] = static_cast<char>(data >> 48 & LAST_BYTE);
        output[7] = static_cast<char>(data >> 56 & LAST_BYTE);
    }

    long long BuffTolong(char* input)
    {
        long long result = 0;
        result += input[0];
        result += static_cast<long long>(input[1]) << 8;
        result += static_cast<long long>(input[2]) << 16;
        result += static_cast<long long>(input[3]) << 24;
        result += static_cast<long long>(input[4]) << 32;
        result += static_cast<long long>(input[5]) << 40;
        result += static_cast<long long>(input[6]) << 48;
        result += static_cast<long long>(input[7]) << 56;
        return result;
    }
private:
    ListNode * head;
    ListNode * tail;
    int count;
};

using namespace boost::unit_test;
BOOST_AUTO_TEST_SUITE(test_suite_main)



BOOST_AUTO_TEST_CASE(printBinaryView_test)
{
    auto res = printBinaryView(53);

    BOOST_CHECK_MESSAGE(res[0] == 1, "wrong bit 0 in binary view");
    BOOST_CHECK_MESSAGE(res[1] == 0, "wrong bit 1 in binary view");
    BOOST_CHECK_MESSAGE(res[2] == 1, "wrong bit 2 in binary view");
    BOOST_CHECK_MESSAGE(res[3] == 0, "wrong bit 3 in binary view");
    BOOST_CHECK_MESSAGE(res[4] == 1, "wrong bit 4 in binary view");
    BOOST_CHECK_MESSAGE(res[5] == 1, "wrong bit 5 in binary view");
    for(size_t i = 6; i < BIT_SIZE; i++)
        BOOST_CHECK_MESSAGE(res[i] == 0, "wrong bit " << i << " in binary view");

    res = printBinaryView(-53);

    BOOST_CHECK_MESSAGE(res[0] == 1, "wrong bit 0 in binary view");
    BOOST_CHECK_MESSAGE(res[1] == 1, "wrong bit 1 in binary view");
    BOOST_CHECK_MESSAGE(res[2] == 0, "wrong bit 2 in binary view");
    BOOST_CHECK_MESSAGE(res[3] == 1, "wrong bit 3 in binary view");
    BOOST_CHECK_MESSAGE(res[4] == 0, "wrong bit 4 in binary view");
    BOOST_CHECK_MESSAGE(res[5] == 0, "wrong bit 5 in binary view");
    for(size_t i = 6; i < BIT_SIZE; i++)
        BOOST_CHECK_MESSAGE(res[i] == 1, "wrong bit " << i << " in binary view");

    res = printBinaryView(-259);

    BOOST_CHECK_MESSAGE(res[0] == 1, "wrong bit 0 in binary view");
    BOOST_CHECK_MESSAGE(res[1] == 0, "wrong bit 1 in binary view");
    BOOST_CHECK_MESSAGE(res[2] == 1, "wrong bit 2 in binary view");
    BOOST_CHECK_MESSAGE(res[3] == 1, "wrong bit 3 in binary view");
    BOOST_CHECK_MESSAGE(res[4] == 1, "wrong bit 4 in binary view");
    BOOST_CHECK_MESSAGE(res[5] == 1, "wrong bit 5 in binary view");
    BOOST_CHECK_MESSAGE(res[6] == 1, "wrong bit 6 in binary view");
    BOOST_CHECK_MESSAGE(res[7] == 1, "wrong bit 7 in binary view");
    BOOST_CHECK_MESSAGE(res[8] == 0, "wrong bit 8 in binary view");
    for(size_t i = 9; i < BIT_SIZE; i++)
        BOOST_CHECK_MESSAGE(res[i] == 1, "wrong bit " << i << " in binary view");
    
    res = printBinaryView(INT_MAX);
    for(size_t i = 0; i < BIT_SIZE - 1; i++)
        BOOST_CHECK_MESSAGE(res[i] == 1, "wrong bit " << i << " in binary view");
    BOOST_CHECK_MESSAGE(res[BIT_SIZE - 1] == 0, "wrong bit " << BIT_SIZE - 1 << " in binary view");

    res = printBinaryView(INT_MIN);
    for(size_t i = 0; i < BIT_SIZE - 1; i++)
        BOOST_CHECK_MESSAGE(res[i] == 0, "wrong bit " << i << " in binary view");
    BOOST_CHECK_MESSAGE(res[BIT_SIZE - 1] == 1, "wrong bit " << BIT_SIZE - 1 << " in binary view");
}

BOOST_AUTO_TEST_CASE(removeDups_test)
{
    char str[] = "AAA BBB AAA";
    char result[] = "A B A";

    removeDups(str);
    BOOST_CHECK_MESSAGE(strcmp(result, str) == 0, "during removing duplicates in " << str << " something wrong happened");

    char str1[] = "A B A";

    removeDups(str1);
    BOOST_CHECK_MESSAGE(strcmp(result, str1) == 0, "during removing duplicates in " << str1 << " something wrong happened");

    char str2[] = "A   B                      A";

    removeDups(str2);
    BOOST_CHECK_MESSAGE(strcmp(result, str2) == 0, "during removing duplicates in " << str2 << " something wrong happened");

    char str3[] = " ABCCFFFGHH %%%BAA";
    char result3[] = " ABCFGH %BA";

    removeDups(str3);
    BOOST_CHECK_MESSAGE(strcmp(result3, str3) == 0, "during removing duplicates in " << str3 << " something wrong happened");
}

BOOST_AUTO_TEST_CASE(list_serialize_deserialize_test)
{

    ListNode* one = new ListNode;
    one->data = std::string("1");

    ListNode* two = new ListNode;
    two->data = std::string("2");

    ListNode* three = new ListNode;
    three->data = std::string("3");

    ListNode* four = new ListNode;
    four->data = std::string("4");

    ListNode* five = new ListNode;
    five->data = std::string("5");

    one->next = two;
    one->rand = three;
    two->prev = one;
    two->next = three;
    two->rand = five;
    three->prev = two;
    three->next = four;
    three->rand = four;
    four->next = five;
    four->prev = three;
    four->rand = two;
    five->prev = four;
    five->rand = one;

    List listToWrite(one, 5);
    FILE * ptrFile = fopen ( "file.txt" , "wb" );
    listToWrite.Serialize(ptrFile);
    fclose(ptrFile);

    List listToRead;
    ptrFile = fopen ( "file.txt" , "rb" );
    listToRead.Deserialize(ptrFile);
    fclose(ptrFile);

    BOOST_CHECK_MESSAGE(listToRead.getCount() == 5, "wrong number of nodes");
    BOOST_CHECK_MESSAGE(listToRead.getTail()->data.compare("5") == 0, "wrong tail");

    int i = 1;
    for (ListNode* cur = listToRead.getHead(); cur != nullptr; cur = cur->next)
    {
        std::string digit = std::to_string(i);
        BOOST_CHECK_MESSAGE(strcmp(cur->data.c_str(), digit.c_str()) == 0, "wrong node " << i);
        i++;
    }

    BOOST_CHECK_MESSAGE(listToRead.getHead()->rand->data.compare("3") == 0, "wrong rand node of first node");
    BOOST_CHECK_MESSAGE(listToRead.getHead()->next->rand->data.compare("5") == 0, "wrong rand node of second node");
    BOOST_CHECK_MESSAGE(listToRead.getHead()->next->next->rand->data.compare("4") == 0, "wrong rand node of third node");
    BOOST_CHECK_MESSAGE(listToRead.getHead()->next->next->next->rand->data.compare("2") == 0, "wrong rand node of fourth node");
    BOOST_CHECK_MESSAGE(listToRead.getHead()->next->next->next->next->rand->data.compare("1") == 0, "wrong rand node of fifth node");
}

BOOST_AUTO_TEST_SUITE_END()
