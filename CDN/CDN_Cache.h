#ifndef CDN_CACHE_H
#define CDN_CACHE_H

#include <iostream>
#include <unordered_map>
using namespace std;

class LRUCache{
   
private:
    typedef struct Node {
        string key;
        long long val;
        Node* next;
        Node* prev;
        Node(string k, long long v) {
            key = k;
            val = v;
            next = NULL;
            prev = NULL;
        }
    } Node;
    
    void update(Node* node) {
        if(head==node)
            return;
        if(node->prev) {
            node->prev->next = node->next;
        }
        if(node->next) {
            node->next->prev = node->prev;
        }
        if(node==tail) {
            tail = tail->prev;
            tail->next = NULL;
        }
        node->next = head;
        node->prev = NULL;
        head->prev = node;
        head = node;
    }
    unordered_map<string, Node*> keyToNodeMap;
    Node* head;
    Node* tail;

public:
    LRUCache() {

        head = NULL;
        tail = NULL;
    }
    
    void get(string key) {
        
        Node *node = keyToNodeMap[key];
        update(node);
    }
    
    
    
    void set(string key, long long value) {
        if(keyToNodeMap.count(key) == 0) {
            Node *newNode = new Node(key, value);
            keyToNodeMap[key] = newNode;
           
            if(head==NULL) {
                head = newNode;
                tail = newNode;
            } else {
                newNode->next = head;
                head->prev = newNode;
                head = newNode;
            }
    } else {
            Node *node = keyToNodeMap[key];
            node->val = value;
            update(node);
        }
    }
    
    string remove() {
        
            keyToNodeMap.erase(tail->key);
            Node* tempTail = tail;
            string filename = tail->key;
            cout << "removed filename is: " << filename << endl;
            tail = tail->prev;
            tail->next = NULL;
            cout << "new last filename is: " << tail->key << endl;
            delete tempTail;
        
            return filename;
    }
    
    void remove(string key) {
        unordered_map<string, Node*>::iterator i = keyToNodeMap.begin();
        while(i != keyToNodeMap.end()){
            if(i->first == key) {
                keyToNodeMap.erase(i->first);
                Node* tempTail = i->second;
                i->second->next->prev = i->second->prev;
                i->second->prev->next = i->second->next;
                delete tempTail;
                break;
            }
            ++i;
        }
    }
    
};

#endif