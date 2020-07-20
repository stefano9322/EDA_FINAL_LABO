#pragma once
#include <functional>
#include <iostream>
#include <random>
#include <vector>
#include <mutex>
#include <atomic>
#include <list>
#include <memory>
#include <limits>

namespace ADE {
	namespace Concurrent {

		template <typename Type, class Comparator = std::less<Type>>
		class SkipList {
		public:
			typedef Type data_type;
			Comparator compare_;

			bool get_random_bool() {
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distribution(0, 1);
				return distribution(gen);
			}

			struct Node {
				Node(unsigned int level, const data_type& data)
					: forward_(level, nullptr), value_(data) {}
				int key;
				std::vector<Node*> forward_;
				data_type value_;
				std::mutex mutex;
				std::atomic<bool> marked;
				std::atomic<bool> fullyLinked;
			};

			explicit SkipList(unsigned int max_level = 16)
				: max_level_(max_level),
				current_level_(1),
				header_(max_level_, data_type()) {}

			void Lock(Node* new_node) {
				new_node->mutex.lock();
			}
			void UnLock(Node* new_node) {
				new_node->mutex.unlock();
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
			bool my_insert(const data_type& data) {
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
				Lock(new_node);
				for (unsigned int i = 0; i < level; ++i) {
					new_node->forward_[i] = predecessor[i]->forward_[i];
					predecessor[i]->forward_[i] = new_node;
				}
				UnLock(new_node);
				return true;
			}

			int randomLevel() const {
				static thread_local std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
				std::uniform_int_distribution<int> distribution(0, 1);
				int lvl = 1;
				while (distribution(generator) && lvl < max_level_) {
					++lvl;
				}
				return lvl;
			}
			bool insert(int key, const std::string& value) {
				int top_layer = randomLevel();
				std::vector<std::shared_ptr<Node>> preds(max_level_ + 1);
				std::vector<std::shared_ptr<Node>> succs(max_level_ + 1);
				while (true) {
					int found_level = 0;
					bool found = findNode(key, &preds, &succs, &found_level);
					if (found) {
						Node* node_found = succs[found_level].get();
						if (!node_found->marked.load()) {
							while (!node_found->fullyLinked.load()) { ; }
							return false;
						}
						else {
							continue;
						}
					}
					{
						std::list<std::unique_ptr<std::lock_guard<std::mutex>>> lock_guards;

						Node* pred = nullptr, * succ = nullptr, * prevPred = nullptr;
						bool valid = true;
						for (int layer = 1; valid && layer <= top_layer; ++layer) {
							pred = preds[layer].get();
							succ = succs[layer].get();
							if (pred != prevPred) {
								lock_guards.push_back(
									std::unique_ptr<std::lock_guard<std::mutex>>(
										new std::lock_guard<std::mutex>(
											pred->mutex
											)
										)
								);
								prevPred = pred;
							}
							valid = !pred->marked.load() && !succ->marked.load() && pred->forward_[layer].get() == succ;
						}
						if (!valid) {
							continue;
						}
						Node* p = new Node(top_layer, data_type(value));
						std::shared_ptr<Node> sp = std::move(p);
						for (int layer = 1; layer <= top_layer; ++layer) {
							sp->forward_[layer] = succs[layer];
							preds[layer]->forward_[layer] = sp;
						}
						sp->fullyLinked.store(true);
						return true;
					}
				}
			}	void debug_print() {
				Node* it = &header_;
				while (it->forward_[0] != nullptr) {
					for (unsigned int i = 0; i < it->forward_.size(); ++i) {
						std::cout << it->forward_[0]->value_ << " ";
					}
					std::cout << std::endl;
					it = it->forward_[0];
				}
			}
			void deleteElement(const data_type& value)
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
				std::cout << "Se elimino el dato: " << value << std::endl;
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
	}  // namespace Concurrent
}  // namespace ADE

