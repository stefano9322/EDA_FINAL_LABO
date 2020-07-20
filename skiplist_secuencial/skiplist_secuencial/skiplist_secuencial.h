#pragma once
#include <functional>
#include <iostream>
#include <random>
#include <vector>

namespace ADE {
	namespace Sequential {

		template <typename Type, class Comparator = std::less<Type>>
		class SkipList {
		public:
			typedef Type data_type;
			Comparator compare_;
			//random
			bool get_random_bool() {
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distribution(0, 1);
				return distribution(gen);
			}
			//node
			struct Node {
				Node(unsigned int level, const data_type& data)
					: forward_(level, nullptr), value_(data) {}
				std::vector<Node*> forward_;
				data_type value_;
			};
			//skiplist
			explicit SkipList(unsigned int max_level = 16)
				: max_level_(max_level),
				current_level_(1),
				header_(max_level_, data_type()) {}
			//insert
			bool insert(const data_type& data) {
				std::vector<Node*> predecessor;
				Node* it = find(data, &predecessor);

				if (it != nullptr && it->value_ == data) {
					return false;
				}

				unsigned int level = 1;
				while (get_random_bool() && level < max_level_) {
					++level;
				}

				if (current_level_ < level) {
					predecessor[current_level_] = &header_;
					level = ++current_level_;
				}

				Node* new_node = new Node(level, data_type(data));
				for (unsigned int i = 0; i < level; ++i) {
					new_node->forward_[i] = predecessor[i]->forward_[i];
					predecessor[i]->forward_[i] = new_node;
				}

				return true;
			}

			unsigned int size() const {
				unsigned int size = 0;
				Node* it = header_.forward_[0];
				while (it != nullptr) {
					++size;
					it = it->forward_[0];
				}
				return size;
			}

			void debug_print() {
				Node* it = &header_;
				while (it->forward_[0] != nullptr) {
					for (unsigned int i = 0; i < it->forward_.size(); ++i) {
						std::cout << it->forward_[0]->value_ << " ";
					}
					std::cout << std::endl;
					it = it->forward_[0];
				}
			}

			bool Delete(const data_type& data) {
				std::vector<Node*> predecessor;
				Node* it = find(data, &predecessor);

				if (it == nullptr || it->value_ != data) {
					return false;
				}

				for (unsigned int i = 0; i < current_level_; ++i) {
					if (predecessor[i]->forward_[i] != it) {
						break;
					}
					predecessor[i]->forward_[i] = it->forward_[i];
				}
				delete it;
				while (1 < current_level_ && header_.forward_[current_level_ - 1] == 0) {
					--current_level_;
				}
				return true;
			}

			void Delete_element(const data_type& value)
			{
				Node* node = &header_;
				Node* update[max_level_ + 1];
				memset(update, 0, sizeof(Node*) * (max_level_ + 1));

				for (int i = current_level_; i >= 0; --i) {
					while (node->forward_[i] != nullptr && compare_(node->forward_[i]->value_, value)) {
						node = node->forward_[i];
					}
					update[i] = node;
				}

				node = node->forward_[0];
				if (node->value_ == value) {
					for (int i = 0; i <= current_level_ - 1; i++) {
						if (update[i]->forward_[i] != node)
							break;
						update[i]->forward_[i] = node->forward_[i];
					}
					delete node;

					while (current_level_ > 0 && header_.forward_[current_level_] == nullptr)
						current_level_--;
				}
				std::cout << "Data deleted " << value << std::endl;
			}
			bool contains(const data_type& data) {
				std::vector<Node*> predecessor;
				Node* it = find(data, &predecessor);
				return (it != nullptr && it->value_ == data);
			}
			void clear() {
				Node* it = header_.forward_[0];
				while (it != nullptr) {
					Node* ptr = it->forward_[0];
					delete it;
					it = ptr;
				}
				for (unsigned int i = 0; i < current_level_; ++i) {
					header_.forward_[i] = nullptr;
				}
				current_level_ = 1;
			}
			bool empty() const { return header_.forward_[0] == nullptr; }

			virtual ~SkipList() {
				Node* it = header_.forward_[0];
				while (it) {
					Node* ptr = it->forward_[0];
					delete it;
					it = ptr;
				}
			}

		private:
			Node* find(const data_type& data, std::vector<Node*>* predecessor) {
				predecessor->resize(max_level_, nullptr);
				Node* it = &header_;
				for (int i = current_level_ - 1; 0 <= i; --i) {
					while (it->forward_[i] != nullptr &&
						compare_(it->forward_[i]->value_, data)) {
						it = it->forward_[i];
					}
					(*predecessor)[i] = it;
				}
				it = it->forward_[0];

				return it;
			}

			unsigned int max_level_;
			unsigned int current_level_;
			Node header_;
		};
	}  // namespace Sequential
}  // namespace ADE

