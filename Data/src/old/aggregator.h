/* 
 * File:   aggregator.h
 * Author: gschoenh
 *
 * Created on March 8, 2013, 11:12 AM
 */

#ifndef AGGREGATOR_H
#define	AGGREGATOR_H
#include<vector>

namespace msii810161816 
{
    namespace data 
    {
        template<typename type>
        class aggregator {
        public:
            aggregator( int _spacing = 1000 ) : spacing(_spacing), size(_spacing)
            {
                std::vector<type> agg(spacing);
                aggregate = agg;
                position = 0;
            }
            virtual ~aggregator() {}
            
            int spacing;      
            int position;         
            void consume(type entry)
            {
                if (position < size)
                {
                    aggregate[position] = entry;
                }                       
                else
                {
                    std::vector<type> newaggregate(spacing);
                    newaggregate.insert(newaggregate.begin(), aggregate.begin(), aggregate.end());
                    aggregate = newaggregate;
                    size = aggregate.size();
                    
                    aggregate[position] = entry;
                }
                position++;
            }
            std::vector<type> retrieve()
            {
                std::vector<type> sub(&aggregate[0], &aggregate[position-1]);
                return sub;
            }
            type retrieve(int index)
            {
                if index < position
                
                return aggregate(index);
            }
        private:
            std::vector<type> aggregate;
            int size;
        };
    }
}

#endif	/* AGGREGATOR_H */

