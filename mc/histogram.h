#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <vector>
#include <ostream>

template <typename Iter>
inline void PrintToStream(std::ostream & stream,Iter it, Iter end) {
    stream << "{";
	bool first = true;
	for (; it!=end; ++it) {
		stream << (first?"":",") << *it;
		first = false;
	}
	stream << "}";
}

template <typename T>
class Histogram {
public:
	Histogram(T min, T max, int bins = 0, bool calculateaverage = true, bool allowresize = true) :
		bins_(bins),
		min_(min),
		max_(max),
		measurements_(0),
		total_(0.0),
		total_squared_(0.0),
		below_min_(0),
		calculateaverage_(calculateaverage),
		allowresize_(allowresize)
	{
        if( bins == 0 ) bins_ = static_cast<int>(max-min);
        
		histogram_.resize(bins_,0);
	}
	void Resize(int bins) {
        T binsize = (max_-min_)/bins_;
		bins_ = bins;
		histogram_.resize(bins_,0);
        max_ = min_ + binsize * bins;
	}
	virtual bool Insert(T x) {
		if( calculateaverage_ ) {
			total_ += static_cast<double>(x);
			total_squared_ += static_cast<double>(x)*static_cast<double>(x);
		}
		measurements_++;

		int bin = Bin(x);

		if( bin < 0 ) below_min_++; 

        else if( bin < bins_ ) { histogram_[bin]++; return true; } 

		else if( allowresize_ ) {
            Resize( bin+1);
            histogram_[bin]++;
            return true;
        }
		return false;
	}
	void PrintTo(std::ostream & stream) const {
		stream << "{min -> " << min_;
		stream << ", max -> " << max_;
		stream << ", bins -> " << bins_;
		stream << ", measurements -> " << measurements_;
		stream << ", belowmin -> " << below_min_;
		if( calculateaverage_ ) {
			stream << ", average -> " << (measurements_ > 0 ? total_/measurements_ : 0.0);
			stream << ", variance -> " << (measurements_ > 0 ? (total_squared_ - total_*total_/measurements_)/measurements_ : 0.0);
		}
		stream << ", histogram -> ";
		PrintToStream( stream, histogram_.begin(), histogram_.end() );
		stream << "}";
	}
	void Reset() {
		std::fill(histogram_.begin(),histogram_.end(),0);
		measurements_ = 0;
		total_ = 0.0;
		total_squared_ = 0.0;
		below_min_ = 0;
	}
	
	T GetMin() const { return min_; }
	T GetMax() const { return max_; }
	T GetAverage() const { return total_/measurements_; }
	T GetStandardDeviation() const { return std::sqrt( (total_squared_ - total_*total_/measurements_)/measurements_ ); }

	int GetBins() const { return bins_; }
	
protected:
	int bins_;
	T min_, max_;

private:
	int Bin(T x) const { return static_cast<int>(((x - min_)*bins_)/(max_-min_)); }

	uint64_t measurements_;
	uint64_t below_min_;

	double total_, total_squared_;

	std::vector<uint64_t> histogram_;
	
	bool calculateaverage_;
    bool allowresize_;
};


std::vector<int> createHistogram(const std::vector<int>& values, int minValue, int maxValue, int binSize, int histLength) {
    int numBins = (maxValue - minValue) / binSize + 1; // Calculate the number of bins needed
    std::vector<int> bins(numBins, 0); // Initialize a vector to hold the bin counts

    // Loop through the values and increment the appropriate bin count
    for (int i = 0; i < values.size(); i++) {
        int binIndex = (values[i] - minValue) / binSize;
        if (binIndex >= 0 && binIndex < numBins) {
            bins[binIndex]++;
        }
    }

    // If the length of the bins array is smaller than histLength, fill it with zeros
    if (bins.size() < histLength) {
        bins.resize(histLength, 0);
    }

    return bins;
}


#endif
