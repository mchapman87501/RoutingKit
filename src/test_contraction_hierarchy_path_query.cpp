#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/min_max.h>
#include <routingkit/timer.h>

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string first_out_file;
		string head_file;
		string weight_file;
		string ch_file;
		string source_file;
		string target_file;
		string distance_file;

		if(argc != 8){
			cerr << argv[0] << " first_out head weight_file ch_file source_file target_file distance_file" << endl;
			return 1;
		}else{
			first_out_file = argv[1];
			head_file = argv[2];
			weight_file = argv[3];
			ch_file = argv[4];
			source_file = argv[5];
			target_file = argv[6];
			distance_file = argv[7];
		}

		cout << "Loading Graph ... " << flush;

		auto first_out = load_vector<unsigned>(first_out_file);
		auto tail = invert_inverse_vector(first_out);
		auto head = load_vector<unsigned>(head_file);
		auto weight = load_vector<unsigned>(weight_file);

		cout << "done" << endl;

		cout << "Loading Contraction Hierarchy ... " << flush;

		ContractionHierarchy ch = ContractionHierarchy::load_file(ch_file);

		cout << "done" << endl;

		cout << "Loading test queries ... " << flush;

		auto source = load_vector<unsigned>(source_file);
		auto target = load_vector<unsigned>(target_file);
		auto ref_distance = load_vector<unsigned>(distance_file);

		cout << "done" << endl;

		const unsigned query_count = source.size();

		cout << "Loaded " << query_count << " test queries" << endl;

		vector<unsigned>distance(query_count);
		ContractionHierarchyQuery ch_query(ch);

		cout << "Running test queries ... " << flush;

		long long dist_time_max = 0;
		long long dist_time_sum = 0;

		long long arc_path_time_max = 0;
		long long arc_path_time_sum = 0;

		long long node_path_time_max = 0;
		long long node_path_time_sum = 0;


		for(unsigned i=0; i<query_count; ++i){

			try{
				long long time = -get_micro_time();
				ch_query.reset().add_source(source[i]).add_target(target[i]).run();
				time += get_micro_time();

				dist_time_max = std::max(dist_time_max, time);
				dist_time_sum += time;

				if(ch_query.get_distance() != ref_distance[i])
					throw runtime_error("Distance query is wrong");

				time = -get_micro_time();
				auto arc_path = ch_query.get_arc_path();
				time += get_micro_time();					

				arc_path_time_max = std::max(arc_path_time_max, time);
				arc_path_time_sum += time;

				time = -get_micro_time();
				auto node_path = ch_query.get_node_path();
				time += get_micro_time();

				node_path_time_max = std::max(node_path_time_max, time);
				node_path_time_sum += time;

				if(source[i] == target[i]){
					if(!arc_path.empty())
						throw runtime_error("if s == t then the path should be empty");
				}else{
					if(arc_path.empty()){
						if(ref_distance[i] != inf_weight)
							throw runtime_error("no path found even though one exists");
					}else{
						if(ch_query.get_used_source() != source[i])
							throw runtime_error("get_used_source returns wrong value");
						if(ch_query.get_used_target() != target[i])
							throw runtime_error("get_used_target returns wrong value");

						if(tail[arc_path.front()] != source[i])
							throw runtime_error("path does not start at the source");
						if(head[arc_path.back()] != target[i])
							throw runtime_error("path does not end at the target");
						for(unsigned j=0; j<arc_path.size()-1; ++j)
							if(head[arc_path[j]] != tail[arc_path[j+1]])
								throw runtime_error("path is not continous");
						unsigned distance = 0;
						for(auto a:arc_path)
							distance += weight[a];
						if(distance != ref_distance[i])
							throw runtime_error("path has the wrong length");

						if(arc_path.size()+1 != node_path.size())
							throw runtime_error("get_node_path path has wrong number of nodes");
						if(node_path.front() != source[i])
							throw runtime_error("get_node_path path does not start at source");
						for(unsigned j=0; j<arc_path.size(); ++j)
							if(head[arc_path[j]] != node_path[j+1])
								throw runtime_error("get_node_path path has a different node than induced by the arc path.");
					}
				}
			}catch(std::runtime_error&err){
				throw std::runtime_error("during query "+std::to_string(i)+" : "+err.what());
			}
		}

		cout << "done" << endl;

		cout << "max dist running time : " << dist_time_max << "musec" << endl;
		cout << "avg dist running time : " << dist_time_sum/query_count << "musec" << endl;

		cout << "max node path running time : " << node_path_time_max << "musec" << endl;
		cout << "avg node path running time : " << node_path_time_sum/query_count << "musec" << endl;

		cout << "max arc path running time : " << arc_path_time_max << "musec" << endl;
		cout << "avg arc path running time : " << arc_path_time_sum/query_count << "musec" << endl;

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
		return 1;
	}
	return 0;
}

