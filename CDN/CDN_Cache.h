#ifndef CDN_CACHE_H
#define CDN_CACHE_H

#include <iostream>
#include <unordered_map>
using namespace std;

class LRUCache{
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
    
    string remove(vector<string>& deletedfiles) {
    		if(tail==NULL)
    			return "";
            keyToNodeMap.erase(tail->key);
            Node* tempTail = tail;
            string filename = tail->key;
            tail = tail->prev;
            if(tail)
            	tail->next = NULL;
            else
            	head = NULL;
            if(tempTail)
            	delete tempTail;
            deletedfiles.push_back(filename);
            return filename;
    }
    
    void remove(string key) {
        if(keyToNodeMap.count(key) > 0) {
        	Node* nodeToRemove = keyToNodeMap[key];
        	if(nodeToRemove->prev)
        		nodeToRemove->prev->next = nodeToRemove->next;
        	if(nodeToRemove->next)
        		nodeToRemove->next->prev = nodeToRemove->prev;
        	if(nodeToRemove == head)
        		head = nodeToRemove->next;
        	if(nodeToRemove == tail)
        		tail = nodeToRemove->prev;
        	keyToNodeMap.erase(key);
        	delete nodeToRemove;
        }
    }

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
};

#endif
