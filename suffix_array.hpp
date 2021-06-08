#ifndef SUFFIX_ARRAY_LIB_SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_LIB_SUFFIX_ARRAY_HPP

#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

namespace sal {

using id_type = int;
using std::vector, std::string;

template<typename It>
vector<id_type> SuffixArray(It begin, It end) {
  using T = typename std::iterator_traits<It>::value_type;
//  for (auto it = begin; it != end; ++it)
//    std::cerr<<*it<<' ';
//  std::cerr<<std::endl;

  int n = end - begin + 1;
  auto get = [&](int i) -> T {
    return i < n-1 ? *(begin + i) : 0;
  };

  if (n == 1) {
    return {0};
  } else if (n == 2) {
    return {1, 0};
  }

  T max_c = *std::max_element(begin, end);
  if (max_c/32 > n) {
    // Character compression
    vector<T> clone(n);
    std::copy(begin, end, clone.begin());
    clone.back() = 0;
    sort(clone.begin(), clone.end());
    clone.erase(unique(clone.begin(), clone.end()), clone.end());
    std::unordered_map<T, T> cmap;
    for (int i = 0; i < clone.size(); i++) {
      cmap[clone[i]] = i;
    }
    clone.resize(n);
    for (int i = 0; i < n; i++)
      clone[i] = cmap[get(i)];
    return SuffixArray(clone.begin(), clone.end());
  }

  vector<id_type> buckets(max_c + 2);
  auto incr_cnt_c = [&](int i) {
    buckets[i+1]++;
  };
  incr_cnt_c(0);
  for (int i = 0; i < n-1; i++)
    incr_cnt_c(get(i));
  for (int i = 1; i < buckets.size(); i++)
    buckets[i] += buckets[i-1];
  vector<id_type> bucket_l(max_c + 1), bucket_r(max_c + 1);
  for (int i = 0; i < buckets.size() - 1; i++) {
    bucket_l[i] = buckets[i];
    bucket_r[i] = buckets[i+1] - 1;
  }

  vector<id_type> sa(n, -1);
  vector<uint8_t> isS(n);
  vector<uint8_t> isLms(n);
  vector<id_type> lms_ids;
  auto set_lms = [&](int i) {
    isLms[i] = true;
    T c = get(i);
    sa[bucket_r[c]--] = i;
    lms_ids.push_back(i);
  };
  isS[n-1] = true;
  set_lms(n-1);
  isS[n-2] = false;
  for (int i = n-3; i >= 0; i--) {
    if (get(i) < get(i+1)) {
      isS[i] = true;
    } else if (get(i) > get(i+1)) {
      if (isS[i+1]) {
        set_lms(i+1);
      }
    } else {
      isS[i] = isS[i+1];
    }
  }
  reverse(lms_ids.begin(), lms_ids.end());

  auto induced_sort = [&]() {
    for (int i = 0; i < n; i++) {
      if (sa[i] == -1) continue;
      int pre = sa[i]-1;
      if (pre < 0)
        pre += n;
      if (!isS[pre]) {
        sa[bucket_l[get(pre)]++] = pre;
      }
      if (isLms[sa[i]]) {
        bucket_r[get(sa[i])]++;
        sa[i] = -1;
      }
    }
    for (int i = n-1; i >= 0; i--) {
      int pre = sa[i] - 1;
      if (pre < 0)
        pre += n;
      if (isS[pre]) {
        sa[bucket_r[get(pre)]--] = pre;
      }
    }
//    for (int i = 0; i < n; i++)
//      std::cerr<<sa[i]<<' ';
//    std::cerr<<std::endl;
  };
  induced_sort();

  vector<id_type> lms_map(n, -1);
  int id = 0;
  int pi = -1;
  bool distinct = true;
  for (int i = 0; i < n; i++) {
    if (!isLms[sa[i]]) continue;
    if (pi != -1) {
      int pf = pi;
      int cf = sa[i];
      bool isSame = true;
      int k = 0;
      while (isSame) {
        isSame = get(pf + k) == get(cf + k) and isLms[pf + k] == isLms[cf + k];
        if (k > 0 and (isLms[pf + k] or isLms[cf + k]))
          break;
        k++;
      }
      if (!isSame) {
        id++;
      } else {
        distinct = false;
      }
    }
    lms_map[sa[i]] = id;
    pi = sa[i];
  }

  if (distinct) {
    vector<int> lmss;
    lmss.reserve(lms_ids.size());
    for (int i = 0; i < n; i++)
      if (isLms[sa[i]])
        lmss.push_back(sa[i]);

    sa.assign(n, -1);
    for (int i = 0; i <= max_c; i++) {
      bucket_l[i] = buckets[i];
      bucket_r[i] = buckets[i+1] - 1;
    }
    for (int i = (int) lmss.size()-1; i >= 0; i--) {
      sa[bucket_r[get(lmss[i])]--] = lmss[i];
    }
    induced_sort();

  } else {
    vector<id_type> lms_substr;
    lms_substr.reserve(lms_ids.size()-1);
    for (int i : lms_ids) {
      if (i == n-1) continue;
      lms_substr.push_back(lms_map[i]);
    }
    auto lms_sa = SuffixArray(lms_substr.begin(), lms_substr.end());

    sa.assign(n, -1);
    for (int i = 0; i <= max_c; i++) {
      bucket_l[i] = buckets[i];
      bucket_r[i] = buckets[i+1] - 1;
    }
    for (int i = (int) lms_sa.size()-1; i >= 0; i--) {
      auto t = lms_ids[lms_sa[i]];
      sa[bucket_r[get(t)]--] = t;
    }
    induced_sort();
  }

  return sa;
}

vector<id_type> SuffixArray(const string& t) {
  return SuffixArray(t.begin(), t.end());
}

vector<id_type> SuffixArray(std::string_view t) {
  return SuffixArray(t.begin(), t.end());
}

vector<id_type> SuffixArray(const char* t) {
  return SuffixArray(std::string_view(t));
}


vector<id_type> Psi(const vector<id_type>& sa) {
  int n = sa.size();
  vector<id_type> isa(n);
  for (int i = 0; i < n; i++) {
    isa[sa[i]] = i;
  }
  vector<id_type> psi(n);
  for (int i = 0; i < n; i++) {
    int nxt = sa[i] + 1;
    if (nxt == n)
      nxt = 0;
    psi[i] = isa[nxt];
  }
  return psi;
}


vector<id_type> FMIndex(const vector<id_type>& sa) {
  int n = sa.size();
  vector<id_type> isa(n);
  for (int i = 0; i < n; i++) {
    isa[sa[i]] = i;
  }
  vector<id_type> psi(n);
  for (int i = 0; i < n; i++) {
    int prev = sa[i] - 1;
    if (prev < 0)
      prev = n-1;
    psi[i] = isa[prev];
  }
  return psi;
}

}

#endif // SUFFIX_ARRAY_LIB_SUFFIX_ARRAY_HPP