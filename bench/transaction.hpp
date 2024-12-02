#pragma once

class transaction
{
public:
    transaction(int id, int client, int size) : id(id), client(client), size(size) {}

    template<class Archive>
    void serialize(Archive &ar, unsigned int /* version */) 
    {
        
    }

private:
    int id;
    int client;
    int size;
};