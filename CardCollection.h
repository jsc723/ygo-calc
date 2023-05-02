#pragma once
#include <vector>
#include <deque>
#include <algorithm>
#include "deck.h"
namespace YGO {
	class CardCollection {
	public:
		virtual std::vector<Card> front(int k) const = 0;
		virtual int pop_front(int k) = 0;
		virtual void push_front(const Card &to_push) = 0;
		virtual int push_front(std::vector<Card>& to_push) = 0;
		virtual Card remove(int index) = 0;
		virtual std::vector<Card> remove_all(const std::vector<int>& indices) = 0;
		virtual const Card& get(int index) = 0;
		virtual std::vector<Card> back(int k) const = 0;
		virtual int pop_back(int k) = 0;
		virtual void push_back(const Card& to_push) = 0;
		virtual int push_back(std::vector<Card>& to_push) = 0;
		virtual void shuffle() = 0;
		virtual size_t size() const = 0;
	};
	class DefaultCardCollection : public CardCollection {
		std::deque<Card> cards;
	public:
		DefaultCardCollection() = default;
		DefaultCardCollection(const DefaultCardCollection& other) = default;
		DefaultCardCollection& operator=(const DefaultCardCollection& other) = default;
		DefaultCardCollection(DefaultCardCollection&& other) = default;
		DefaultCardCollection& operator=(DefaultCardCollection&& other) = default;

		template<class RandomIt>
		DefaultCardCollection(RandomIt begin, RandomIt end)
		: cards(begin, end) {
		}

		virtual std::vector<Card> front(int k) const {
			std::vector<Card> res;
			for (int i = 0; i < k && i < cards.size(); i++) {
				res.push_back(cards[i]);
			}
			return res;
		};

		virtual int pop_front(int k) {
			int cnt = 0;
			for (int i = 0; i < k && i < cards.size(); i++) {
				cnt++;
				cards.pop_front();
			}
			return cnt;
		}

		void push_front(const Card& to_push) {
			cards.push_front(to_push);
		}

		virtual int push_front(std::vector<Card>& to_push) {
			for (auto c : cards) {
				cards.push_front(c);
			}
			return to_push.size();
		}

		virtual Card remove(int index);
		virtual std::vector<Card> remove_all(const std::vector<int>& indices);
		virtual const Card &get(int index);

		virtual std::vector<Card> back(int k) const {
			std::vector<Card> res;
			int start = std::max<int>(0, (int)cards.size() - k);
			for (int i = start; i < cards.size(); i++) {
				res.push_back(cards[i]);
			}
			return res;
		}

		virtual int pop_back(int k) {
			int cnt = 0;
			while (cards.size() > 0) {
				cnt++;
				cards.pop_back();
			}
			return cnt;
		}

		void push_back(const Card& to_push) {
			cards.push_back(to_push);
		}

		virtual int push_back(std::vector<Card>& to_push) {
			for (auto c : cards) {
				cards.push_back(c);
			}
			return to_push.size();
		}

		virtual void shuffle();

		virtual size_t size() const {
			return cards.size();
		}
	};
}

