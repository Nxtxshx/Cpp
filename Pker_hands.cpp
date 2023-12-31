#include <string>
#include <iostream>

const unsigned long long Card2 = 1ULL <<  0;
const unsigned long long Card3 = 1ULL <<  1;
const unsigned long long Card4 = 1ULL <<  2;
const unsigned long long Card5 = 1ULL <<  3;
const unsigned long long Card6 = 1ULL <<  4;
const unsigned long long Card7 = 1ULL <<  5;
const unsigned long long Card8 = 1ULL <<  6;
const unsigned long long Card9 = 1ULL <<  7;
const unsigned long long CardT = 1ULL <<  8;
const unsigned long long CardJ = 1ULL <<  9;
const unsigned long long CardQ = 1ULL << 10;
const unsigned long long CardK = 1ULL << 11;
const unsigned long long CardA = 1ULL << 12;

// convert a card to a 52-bitmask
unsigned long long cardMask(const std::string& card) // card format is 5H for 5 of hearts
{
  // bit mask structure:
  // bits from  0 to 12 are 23456789TJQKA of diamonds
  // bits from 13 to 25 are 23456789TJQKA of hearts
  // bits from 26 to 38 are 23456789TJQKA of spades
  // bits from 39 to 51 are 23456789TJQKA of clubs
  // bits 52+ are zero
  unsigned long long result = 0;
  switch (card[0])
  {
    case '2': result = Card2; break;
    case '3': result = Card3; break;
    case '4': result = Card4; break;
    case '5': result = Card5; break;
    case '6': result = Card6; break;
    case '7': result = Card7; break;
    case '8': result = Card8; break;
    case '9': result = Card9; break;
    case 'T': result = CardT; break;
    case 'J': result = CardJ; break;
    case 'Q': result = CardQ; break;
    case 'K': result = CardK; break;
    case 'A': result = CardA; break;
    default: break;
  }
  switch (card[1])
  {
    case 'D':                break;
    case 'H': result <<= 13; break;
    case 'S': result <<= 26; break;
    case 'C': result <<= 39; break;
    default: break;
  }

  return result;
}

// each hand with a certain rank beats all hands with a higher rank
unsigned long long rank(unsigned long long hand)
{
  // set the lowest 13 bits (= 13 cards of a suit)
  const auto Suit = (1LL << 13) - 1;

  // ignore color (convert all cards to diamonds)
  auto colorless = (hand | (hand >> 13) | (hand >> 26) | (hand >> 39)) & Suit;

  // greater zero if straight (better straights get higher value)
  unsigned int straight = 0;
  if (colorless == (CardT | CardJ | CardQ | CardK | CardA)) straight =  1;
  if (colorless == (Card9 | CardT | CardJ | CardQ | CardK)) straight =  2;
  if (colorless == (Card8 | Card9 | CardT | CardJ | CardQ)) straight =  3;
  if (colorless == (Card7 | Card8 | Card9 | CardT | CardJ)) straight =  4;
  if (colorless == (Card6 | Card7 | Card8 | Card9 | CardT)) straight =  5;
  if (colorless == (Card5 | Card6 | Card7 | Card8 | Card9)) straight =  6;
  if (colorless == (Card4 | Card5 | Card6 | Card7 | Card8)) straight =  7;
  if (colorless == (Card3 | Card4 | Card5 | Card6 | Card7)) straight =  8;
  if (colorless == (Card2 | Card3 | Card4 | Card5 | Card6)) straight =  9;
  if (colorless == (CardA | Card2 | Card3 | Card4 | Card5)) straight = 10;

  // pairs, triple, fours
  unsigned int count[13] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
  for (unsigned int i = 0; i < 13; i++)
  {
    if (hand & (1ULL <<   i))
      count[i]++;
    if (hand & (1ULL << (i+13)))
      count[i]++;
    if (hand & (1ULL << (i+26)))
      count[i]++;
    if (hand & (1ULL << (i+39)))
      count[i]++;
  }

  // true, if all cards share the same colors
  bool isFlush = (hand ==  colorless)        ||
                 (hand == (colorless << 13)) ||
                 (hand == (colorless << 26)) ||
                 (hand == (colorless << 39));

  // allocate 10000000000 IDs per category (flush, straight, pairs, etc.)
  const unsigned long long GroupSize = 10000000000ULL; // burn up to two digits per card
  unsigned long long result = 0;

  // royal flush and straight flush
  if (isFlush && straight > 0)
    return result + straight;
  result += GroupSize;

  // four-of-a-kind
  for (unsigned int i = 0; i < 13; i++)
    if (count[i] == 4)
      for (unsigned int j = 0; j < 13; j++)
        if (count[j] == 1)
          return result + (13 - i) * 100 + (13 - j);
  result += GroupSize;

  // full-house
  for (unsigned int i = 0; i < 13; i++)
    if (count[i] == 3)
      for (unsigned int j = 0; j < 13; j++)
        if (count[j] == 2)
          return result + (13 - i) * 100 + (13 - j);
  result += GroupSize;

  // flush
  if (isFlush)
  {
    unsigned long long value = 0;
    for (int i = 12; i >= 0; i--)
      if (count[i] == 1)
      {
        value *= 100;
        value += 13 - i;
      }
    return result + value;
  }
  result += GroupSize;

  // straight
  if (straight > 0)
    return result + straight;
  result += GroupSize;

  // three
  for (unsigned int i = 0; i < 13; i++)
    if (count[i] == 3)
    {
      unsigned long long value = 13 - i;
      for (int j = 12; j >= 0; j--)
        if (count[j] == 1)
        {
          value *= 100;
          value += 13 - j;
        }
      return result + value;
    }
  result += GroupSize;

  // one or two pairs
  unsigned int numPairs = 0;
  for (unsigned int i = 0; i < 13; i++)
    if (count[i] == 2)
      numPairs++;
  if (numPairs > 0)
  {
    unsigned long long value = 0;
    // pairs
    for (int i = 12; i >= 0; i--)
      if (count[i] == 2)
      {
        value *= 100;
        value += 13 - i;
      }
    // single card(s)
    for (int i = 12; i >= 0; i--)
      if (count[i] == 1)
      {
        value *= 100;
        value += 13 - i;
      }

    if (numPairs == 1)
      result += GroupSize;
    return result + value;
  }
  result += 2 * GroupSize; // one and two pairs

  // high card
  unsigned long long value = 0;
  for (int i = 12; i >= 0; i--)
    if (count[i] == 1)
    {
      value *= 100;
      value += 13 - i;
    }

  return result + value;
}

int main()
{
  unsigned int tests = 1000;

//#define ORIGINAL
#ifdef ORIGINAL
  unsigned int won1 = 0;
  unsigned int won2 = 0;
#else
  std::cin >> tests;
#endif

  while (tests--)
  {
    std::string cards1[5], cards2[5];
    std::cin >> cards1[0] >> cards1[1] >> cards1[2] >> cards1[3] >> cards1[4];
    std::cin >> cards2[0] >> cards2[1] >> cards2[2] >> cards2[3] >> cards2[4];

    // convert to bitmask and merge with logical OR
    auto player1 = cardMask(cards1[0]) | cardMask(cards1[1]) | cardMask(cards1[2]) |
                   cardMask(cards1[3]) | cardMask(cards1[4]);
    auto player2 = cardMask(cards2[0]) | cardMask(cards2[1]) | cardMask(cards2[2]) |
                   cardMask(cards2[3]) | cardMask(cards2[4]);

    // lower rank wins
    auto rank1 = rank(player1);
    auto rank2 = rank(player2);
#ifdef ORIGINAL
    if (rank1 < rank2)
      won1++;
    if (rank1 > rank2)
      won2++;
#else
    std::cout << "Player " << (rank1 < rank2 ? "1" : "2") << std::endl;
#endif
  }

#ifdef ORIGINAL
  std::cout << won1 << std::endl;
  //std::cout << won2 << std::endl;
#endif

  return 0;
}