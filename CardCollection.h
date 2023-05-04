#pragma once
#include <vector>
#include <list>
#include <algorithm>
#include "deck.h"
namespace YGO {
	using CardNode = std::list<Card>::iterator;

	class CardCollection {
	protected:
		t_string m_name;
		CardCollection(t_string name) : m_name(name) {}

	public:
		virtual t_string name() const { return m_name; }
		virtual CardNode begin() = 0;
		virtual CardNode end() = 0;
		virtual std::vector<Card> to_vector() const = 0;
		virtual void pop_front(int k, std::list<Card> &dst) = 0;
		virtual void push_front(std::list<Card> &other, CardNode &to_push) = 0;
		virtual void push_front(std::list<Card>& to_push) = 0;
		virtual void move_to(CardNode &card_node, std::list<Card>& dst) = 0;
		virtual void move_to(CardNode& card_node, CardCollection& dst) = 0;
		virtual void pop_back(int k, std::list<Card>& dst) = 0;
		virtual void push_back(std::list<Card>& other, CardNode& to_push) = 0;
		virtual void push_back(std::list<Card>& to_push) = 0;
		virtual void shuffle() = 0;
		virtual size_t size() const = 0;
	};

	class DefaultCardCollection : public CardCollection {
		std::list<Card> cards;
	public:
		DefaultCardCollection(t_string name): CardCollection(name) {}
		DefaultCardCollection(const DefaultCardCollection& other) = default;
		DefaultCardCollection& operator=(const DefaultCardCollection& other) = default;
		DefaultCardCollection(DefaultCardCollection&& other) = default;
		DefaultCardCollection& operator=(DefaultCardCollection&& other) = default;

		template<class RandomIt>
		DefaultCardCollection(t_string name, RandomIt begin, RandomIt end)
			:CardCollection(name) ,cards(begin, end) {
		}

		virtual CardNode begin() override {
			return cards.begin();
		}
		virtual CardNode end() override {
			return cards.end();
		}

		virtual std::vector<Card> to_vector() const override {
			return std::vector<Card>(cards.begin(), cards.end());
		}


		virtual void pop_front(int k, std::list<Card>& dst) override {
			k = std::min<int>(k, cards.size());
			auto end = cards.begin();
			while (k-- > 0) {
				end++;
			}
			dst.splice(dst.end(), cards, cards.begin(), end);
		}

		void push_front(std::list<Card>& other, CardNode& to_push) override {
			cards.splice(cards.begin(), other, to_push);
		}

		virtual void push_front(std::list<Card>& other) override {
			cards.splice(cards.begin(), other);
		}

		virtual void move_to(CardNode& card_node, std::list<Card>& dst) override;
		virtual void move_to(CardNode& card_node, CardCollection& dst) override;


		virtual void pop_back(int k, std::list<Card>& dst) override {
			k = std::min<int>(k, cards.size());
			auto start = cards.end();
			while (k-- > 0) {
				start--;
			}
			dst.splice(dst.end(), cards, start, cards.end());
		}

		virtual void push_back(std::list<Card>& other, CardNode& to_push) override
		{
			cards.splice(cards.end(), other, to_push);
		}

		virtual void push_back(std::list<Card>& other) override
		{
			cards.splice(cards.end(), other);
		}

		virtual void shuffle();

		virtual size_t size() const {
			return cards.size();
		}
	};
}

