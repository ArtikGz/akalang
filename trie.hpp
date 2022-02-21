#pragma once
#include "token.hpp"
#include <memory>

#define CHAR_AMOUNT 100

#define get_child_node(node, str, index) node->childs[static_cast<int>(str[index] - '!')]

class Node {
public:
  Token::Type type;
  std::shared_ptr<Node> childs[CHAR_AMOUNT];
};

class Trie {
public:
  std::shared_ptr<Node> root;

  /**
   * @brief Construct a new Trie object, initializes root childs to std::shared_ptr<Node>(nullptr)
   */
  Trie();
  
  /**
   * @brief Asign a keyword to a type on the Trie
   * 
   * @param keyword 
   * @param type 
   */
  void add_keyword(std::string&& keyword, Token::Type type);

  /**
   * @brief Lookup for the most longer keyword found in the text provided
   * 
   * @param text
   * @return Token::Type, Token::Type::UNKNOWN if keyword wasn't found
   */
  Token::Type lookup(std::string& text);
};
