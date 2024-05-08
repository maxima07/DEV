#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <optional>

using namespace std;

const double EPSILON = 1e-6;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}    

struct Document {
    Document() = default;

    Document(int id, double relevance, int rating)
        : id(id)
        , relevance(relevance)
        , rating(rating) {
    }

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

ostream& operator<<(ostream& out, const Document& document){
        out << "{ " 
            << "document_id = " << document.id << ", "
            << "relevance = " << document.relevance << ", "
            << "rating = " << document.rating
            << " }";
        return out;
    }

template <typename StringContainer>
set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    set<string> non_empty_strings;
    for (const string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
    
    public:   
        
        template <typename StringContainer>
        explicit SearchServer(const StringContainer& stop_words)
            : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {

            if(any_of(stop_words.begin(), stop_words.end(), [](const string& word){return !IsValidWord(word);})){
                throw invalid_argument("Stop words have special symbols!"s);
            }      
        }

        explicit SearchServer(const string& stop_words_text)
            : SearchServer(SplitIntoWords(stop_words_text)){ // Invoke delegating constructor from string container
        }
        
        void AddDocument(int document_id, const string& document, DocumentStatus status,
                        const vector<int>& ratings) {
            if(document_id < 0 || count(document_ids_.begin(), document_ids_.end(), document_id)){
                throw invalid_argument("Document ID is wrong or a document with this ID has already been added earlier");
            }
            
            const vector<string> words = SplitIntoWordsNoStop(document);
            const double inv_word_count = 1.0 / words.size();
            
            for (const string& word : words) {
                if(!IsValidWord(word)){
                    throw invalid_argument("The document has invalid characters"s);
                }
                word_to_document_freqs_[word][document_id] += inv_word_count;
            }
            
            documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
            document_ids_.push_back(document_id);
        }

        template <typename DocumentPredicate>
        vector<Document> FindTopDocuments(const string& raw_query,
                                        DocumentPredicate document_predicate) const {
            Query query = ParseQuery(raw_query);
            vector<Document> matched_documents = FindAllDocuments(query, document_predicate);

            sort(matched_documents.begin(), matched_documents.end(),
                [](const Document& lhs, const Document& rhs) {
                    if (abs(lhs.relevance - rhs.relevance) < EPSILON) {
                        return lhs.rating > rhs.rating;
                    } else {
                        return lhs.relevance > rhs.relevance;
                    }
                });
            if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
                matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
            }
            return matched_documents;
        }

        vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
            return FindTopDocuments(
                raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
                    return document_status == status;
                });
        }

        vector<Document> FindTopDocuments(const string& raw_query) const {
            return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
        }

        int GetDocumentCount() const {
            return documents_.size();
        }

        int GetDocumentId(int index) const {
            return document_ids_.at(index);
        }
        
        tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query,
                                                            int document_id) const {
            Query query = ParseQuery(raw_query);
            vector<string> matched_words;
            
            for (const string& word : query.plus_words) {
                if (word_to_document_freqs_.count(word) == 0) {
                    continue;
                }
                if (word_to_document_freqs_.at(word).count(document_id)) {
                    matched_words.push_back(word);
                }
            }

            for (const string& word : query.minus_words) {
                if (word_to_document_freqs_.count(word) == 0) {
                    continue;
                }
                
                if (word_to_document_freqs_.at(word).count(document_id)) {
                    matched_words.clear();
                    break;
                }
            }
            return {matched_words, documents_.at(document_id).status};
        }

    private:

        struct DocumentData {
            int rating;
            DocumentStatus status;
        };

        const set<string> stop_words_;
        
        map<string, map<int, double>> word_to_document_freqs_;
        
        map<int, DocumentData> documents_;

        vector<int> document_ids_;

        bool IsStopWord(const string& word) const {
            return stop_words_.count(word) > 0;
        }

        vector<string> SplitIntoWordsNoStop(const string& text) const {
            vector<string> words;
            for (const string& word : SplitIntoWords(text)) {
                if (!IsStopWord(word)) {
                    words.push_back(word);
                }
            }
            return words;
        }

        static int ComputeAverageRating(const vector<int>& ratings) {
            if (ratings.empty()) {
                return 0;
            }
            int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);
            return rating_sum / static_cast<int>(ratings.size());
        }

        struct QueryWord {
            string data;
            bool is_minus;
            bool is_stop;
        };

        QueryWord ParseQueryWord(string text) const {
            bool is_minus = false;
            if(!IsValidWord(text)){
                throw invalid_argument("The request has invalid characters");
            }

            if(!IsValidWord(text) || (text[0] == '-' && text.size() == 1) || (text[0] == '-' && text[1] == '-')){
                    throw invalid_argument("The query has extra characters before or after the word");
            }

            // Word shouldn't be empty
            if (text[0] == '-') {
                is_minus = true;
                text = text.substr(1);
            }
            return {text, is_minus, IsStopWord(text)};
        }

        struct Query {
            set<string> plus_words;
            set<string> minus_words;
        };

        Query ParseQuery(const string& text) const {
            Query query;
        
            for (const string& word : SplitIntoWords(text)) {
                QueryWord query_word = ParseQueryWord(word);

                if (!query_word.is_stop) {
                    if (query_word.is_minus) {
                        query.minus_words.insert(query_word.data);
                    } else {
                        query.plus_words.insert(query_word.data);
                    }
                }
            }
            return query;
        }

        static bool IsValidWord(const string& word) {
            // A valid word must not contain special characters
            return none_of(word.begin(), word.end(), [](char c) {
                return c >= '\0' && c < ' ';
            });
        }

        // Existence required
        double ComputeWordInverseDocumentFreq(const string& word) const {
            return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
        }

        template <typename DocumentPredicate>
        vector<Document> FindAllDocuments(const Query& query,
                                        DocumentPredicate& document_predicate) const {
            map<int, double> document_to_relevance;
            for (const string& word : query.plus_words) {
                if (word_to_document_freqs_.count(word) == 0) {
                    continue;
                }
                const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
                for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word)) {
                    const auto& document_data = documents_.at(document_id);
                    if (document_predicate(document_id, document_data.status, document_data.rating)) {
                        document_to_relevance[document_id] += term_freq * inverse_document_freq;
                    }
                }
            }

            for (const string& word : query.minus_words) {
                if (word_to_document_freqs_.count(word) == 0) {
                    continue;
                }
                for (const auto &[document_id, _] : word_to_document_freqs_.at(word)) {
                    document_to_relevance.erase(document_id);
                }
            }

            vector<Document> matched_documents;
            for (const auto &[document_id, relevance] : document_to_relevance) {
                matched_documents.push_back(
                    {document_id, relevance, documents_.at(document_id).rating});
            }
            return matched_documents;
        }
};

//Класс хранения итераторов одной страницы
template <typename Iterator>
class IteratorRange {
    public:
        //Неявный конструктор
        IteratorRange(){
        }

        //Явный конструктор
        IteratorRange(const Iterator& begin, const Iterator& end, const size_t size) 
            : begin_(begin)
            , end_(end)
            , size_(size){
        }

        //Итератор на начало контейнера
        Iterator begin() const {
            return begin_;
        }

        //Итератор на конец контейнера
        Iterator end() const {
            return end_;
        }

        //Размер контейнера
        size_t size() const {
            return size_;
        }

    private:
        Iterator begin_; 
        Iterator end_;
        size_t size_;
};

//Класс хранения вектора страниц
template <typename Iterator>
class Paginator {
    public:
        //неявный конструктор
        Paginator(){
        }

        //Явный конструктор
        explicit Paginator (Iterator it_begin, Iterator it_end, size_t page_size){
                
            size_t container_size = distance(it_begin, it_end);

            while (container_size >= page_size){
                Iterator page_begin = it_begin;
                advance (it_begin, page_size);
                Iterator page_end = it_begin;
                pages_.push_back({page_begin, page_end, page_size});
                container_size = distance(it_begin, it_end);
            }
            if (distance(it_begin, it_end) > 0) {
                size_t dist = distance(it_begin, it_end);
                pages_.push_back({it_begin, it_end, dist});
            }
        }

        auto begin() const {
            return pages_.begin();
        }

        auto end() const {
            return pages_.end();
        }

        size_t size() const {
            return pages_.size();
        }
    private:
        vector<IteratorRange<Iterator>> pages_;
};

template <typename Iterator>
ostream& operator<<(ostream& out, const IteratorRange<Iterator>& range) {
    for(auto it = range.begin(); it != range.end(); ++it){
        out << *it;
    }
    return out; 
}

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

int main() {
    SearchServer search_server("and with"s);
    search_server.AddDocument(1, "funny pet and nasty rat"s, DocumentStatus::ACTUAL, {7, 2, 7});
    search_server.AddDocument(2, "funny pet with curly hair"s, DocumentStatus::ACTUAL, {1, 2, 3});
    search_server.AddDocument(3, "big cat nasty hair"s, DocumentStatus::ACTUAL, {1, 2, 8});
    search_server.AddDocument(4, "big dog cat Vladislav"s, DocumentStatus::ACTUAL, {1, 3, 2});
    search_server.AddDocument(5, "big dog hamster Borya"s, DocumentStatus::ACTUAL, {1, 1, 1});
    const auto search_results = search_server.FindTopDocuments("curly dog"s);
    int page_size = 2;
    const auto pages = Paginate(search_results, page_size);
    // Выводим найденные документы по страницам
    for (auto page = pages.begin(); page != pages.end(); ++page) {
        cout << *page << endl;
        cout << "Page break"s << endl;
    }
} 