


#ifndef TUNEIT_H_
#define TUNEIT_H_

#include <bits/stdc++.h>

namespace tuneit
{

	//Common
	/*{{{*/
	struct binary_number{
		binary_number(size_t const num_dimensions):data(num_dimensions,0){}
		std::vector<bool> data;
		bool increment(){
			/*{{{*/
			for (auto it=end(data)-1; it!=begin(data)-1; --it) {
				*it=!(*it);
				if(*it)
					return true;
			}
			return false;
			/*}}}*/
		}
	};

	template<size_t ...> struct seq {};

	template<size_t N, size_t ...S> struct gens : gens<N-1, N-1, S...> {};

	template<size_t ...S> struct gens<0, S...>{ typedef seq<S...> type; };

#ifdef SCIENTIFIC_EXPERIMENT
	size_t sampled_count=0;
#endif

	/*}}}*/

	//Interface: setting
	/*{{{*/
	template <size_t Dim, size_t Num_Variants>
		struct tuneit_settings{

			//tuneit_settings(std::vector<bool> _mask=std::vector<bool>(Num_Variants, true)):mask(_mask){};

			size_t const depth;
			std::vector<bool> const mask;//=std::vector<bool>(Num_Variants, true);
			bool const implementation_pruning;
			bool const oversampling;
			bool const discard_first_measurement;
			size_t const num_measure_per_point;
			size_t const training_range[Dim][2];
		};
	/*}}}*/

	//tree structure
	/*{{{*/
	template <size_t Dim, size_t Num_Variants>
		struct vertex{
			//not store raw performance data,
			//those raw data should be stored in database

			std::array<size_t, Num_Variants> winners;
			//tuple could be better, but for now use homogeneous size_t
			std::array<size_t, Dim > tunable_args;
		};

	template <size_t Dim, size_t Num_Variants, size_t Num_Vertices, class MeasureType>
		struct node{


			explicit node(tuneit_settings<Dim, Num_Variants> const &_settings):  settings(_settings){
				std::copy(&_settings.training_range[0][0], &_settings.training_range[0][0]+Dim*2, &training_range[0][0]);
			}
			explicit node(tuneit_settings<Dim, Num_Variants> const &_settings, size_t _training_range[][2]):  settings(_settings){
				std::copy(&_training_range[0][0], &_training_range[0][0]+Dim*2, &training_range[0][0]);
			}

			//static constexpr size_t num_vertices=pow(2,Dim);
			tuneit_settings<Dim, Num_Variants> const &settings;
			std::vector<node<Dim, Num_Variants, Num_Vertices, MeasureType> > children;
			vertex<Dim, Num_Variants> vertices[Num_Vertices];
			size_t training_range[Dim][2];
			bool closedness;

			void grow_vertices(){


				binary_number bin(Dim);

				for(size_t v=0; v<Num_Vertices; ++v, bin.increment()){
					//for each point
					//here convert jump to a indirect array reference,
					//who knows if it is worthwhile in general,
					//but in this case it is likely to be good
					for (size_t i = 0; i < Dim; i++)
						vertices[v].tunable_args[i]= training_range[i][ bin.data[i] ] ;

				}


			}

			void print_one_node(std::ostream &os, size_t const my_id){
				/*{{{*/
				os<<"\tnode"<<my_id<<std::endl;
				os<<"\t["<<std::endl;
				os<<"\t\tlabel = <<table border=\"0\" cellspacing=\"0\">"<<std::endl;

				std::string color_string;
				if(closedness)
					color_string="bgcolor=\"green\"";

				//print the first half of vertices
				size_t v;
				for(v=0; v<Num_Vertices/2; ++v){

					os<<"\t\t\t       <tr><td port=\"port"<<v<<"\"  border=\"1\" "<<color_string<<">Vertex: [";

					os<<vertices[v].tunable_args[0];
					for(size_t d=1; d<Dim; ++d){
						os<<", "<<vertices[v].tunable_args[d];
					}
					os<<"] [";

					os<<vertices[v].winners[0];

					os<<"]</td></tr>"<<std::endl;

				}

				//print node
				os<<"\t\t\t       <tr><td port=\"port"<<v<<"\"  border=\"1\" "<<color_string<<">Node:";
				for(size_t d=0; d<Dim; ++d)
					os<<" ["<<training_range[d][0]<<" ,"<<training_range[d][1]<<"]";
				os<<"</td></tr>"<<std::endl;

				//print the second half of vertices
				for(v=Num_Vertices/2; v<Num_Vertices; ++v){

					os<<"\t\t\t       <tr><td port=\"port"<<v+1<<"\"  border=\"1\" "<<color_string<<">Vertex: [";

					os<<vertices[v].tunable_args[0];
					for(size_t d=1; d<Dim; ++d){
						os<<", "<<vertices[v].tunable_args[d];
					}
					os<<"] [";

					os<<vertices[v].winners[0];

					os<<"]</td></tr>"<<std::endl;

				}


				os<<"\t        </table>>"<<std::endl;
				os<<"\t]"<<std::endl;
				/*}}}*/
			}
			void print_one_node_with_link(std::ostream &os, size_t const father_id, size_t const my_id, size_t &next_available_id){
				/*{{{*/
				print_one_node(os, my_id);

				//print link
				size_t const middle_port=Num_Vertices/2;
				os<<"\tnode"<<father_id<<":port"<< middle_port <<" -> node"<<my_id<<":"<<"port"<< middle_port <<std::endl;

				//roll the recursive dice
				for(auto it=children.begin(); it!=children.end(); ++it){
					++next_available_id;
					it->print_one_node_with_link(os, my_id, next_available_id, next_available_id);
				}
				/*}}}*/
			}

			template <class Tunable>
				void measure(Tunable const &impls, node<Dim, Num_Variants, Num_Vertices, MeasureType>  const * const father){

					//measure all the vertices using impls
					for(size_t v=0; v<Num_Vertices;++v){

						//if already measured by father node, just update the result
						if(father!=NULL && find_and_update_winner(vertices[v], father))
							continue;

						//extract parameters and pass it impls.run() function
						vertices[v].winners= run_and_get_winner(impls, vertices[v].tunable_args, typename gens<Dim>::type());
					}
				}

			bool find_and_update_winner( vertex<Dim, Num_Variants> &vertex, node<Dim, Num_Variants, Num_Vertices, MeasureType>  const * const father){


				for(auto const &i:father->vertices){
					if(i.tunable_args == vertex.tunable_args){
						vertex.winners=i.winners;
						return true;
					}
				}

				return false;

			}

			template<class Tunable, class T, size_t ...S>
				std::array<size_t, Num_Variants> run_and_get_winner(Tunable const &impls, T const &args_arr, seq<S...>){
					/*{{{*/

#ifdef SCIENTIFIC_EXPERIMENT
				++sampled_count;
#endif

				//std::cerr<<"run_and_get_winner calling..."<<std::endl;

					//connection point
					auto raw_data=impls.training_run(settings.mask, settings.num_measure_per_point, args_arr[S]...);

				//std::cerr<<"GOOD..."<<std::endl;

					//calculate proportions

					//pair format: winner, best_measured_so_far
					std::vector<std::pair<size_t, MeasureType> >  winners_so_far(settings.num_measure_per_point);


					//the first iteration, assign
					for(size_t i=0;i<settings.num_measure_per_point; ++i)
						winners_so_far[i]=std::make_pair<size_t, MeasureType>(static_cast<size_t>(std::get<1>(raw_data[i])), static_cast<MeasureType>(std::get<2>(raw_data[i])));

					//the rest iteration, compare and reduce
					size_t current_line;
					for(size_t v=1;v<Num_Variants;++v){
						for(size_t i=0;i<settings.num_measure_per_point; ++i){
							current_line=v*settings.num_measure_per_point+i;
							if(std::get<2>(raw_data[current_line])<winners_so_far[i].second)
								winners_so_far[i]=std::make_pair<size_t, MeasureType>(static_cast<size_t>(std::get<1>(raw_data[current_line])), static_cast<MeasureType>(std::get<2>(raw_data[current_line])));
						}

					}

					//now you have a vector of winners, you need reduction by count
					//winner, count
					std::array<std::pair<size_t, size_t>, Num_Variants > statistics;
					size_t index=0;
					generate(statistics.begin(), statistics.end(), [&index]{return std::make_pair<size_t, size_t>(index++, 0); });

					for(size_t i=0;i<settings.num_measure_per_point; ++i){
						statistics[winners_so_far[i].first].second++;
					}

					//sort in decreasing order, thus > is
					//used instead of <
					std::sort(statistics.begin(), statistics.end(), [](std::pair<size_t, size_t>&x1, std::pair<size_t, size_t>&x2){ return x1.second>x2.second;});

					std::array<size_t, Num_Variants> final_form;
					index=0;
					std::generate(final_form.begin(), final_form.end(), [&statistics, &index]{return statistics[index++].first;});



					return final_form;
					/*}}}*/
				}


			void update_closeness(){
				/*{{{*/
				//now you know winners on all vertices
				//calculate closeness
				static_assert(Num_Vertices>0, "");
				closedness=true;
				auto current_winner= vertices[0].winners[0];
				for(auto i=1; i<Num_Vertices; ++i){
					if(vertices[i].winners[0]!=current_winner){
						closedness=false;
						break;
					}
				}
				/*}}}*/
			}

			void grow_children(){
				/*{{{*/
				//assume closedness is already checked

				//reserve Num_Vertices, but may not use all of
				//them, due to some dimension may be empty
				children.reserve(Num_Vertices);

				//split the node into Num_Vertices nodes
				//no need to consider the case if one dimension only have 1
				//input element left
				//as usually the training can not go that deep
				//even if some dimension are small,
				//the repeated value will not be measured,
				//but inherited from father vertices
				size_t all_extreme_points[Dim][4], half;

				for(size_t i=0;i<Dim;++i){
					all_extreme_points[i][0]=training_range[i][0];
					all_extreme_points[i][3]=training_range[i][1];
					half=size_t( (training_range[i][1]-training_range[i][0]) /2);
					all_extreme_points[i][1]=training_range[i][0]+half;
					all_extreme_points[i][2]=std::min(all_extreme_points[i][1]+1,training_range[i][1]);
				}

				binary_number bin(Dim);
				size_t _training_range[Dim][2];

				do{
					for(size_t i=0;i<Dim;++i){
						_training_range[i][0]=all_extreme_points[i][2*bin.data[i]];
						_training_range[i][1]=all_extreme_points[i][2*bin.data[i]+1];
					}
					children.emplace_back(settings, _training_range);
				}while(bin.increment());

				/*}}}*/
			}

			bool contains(std::array<size_t, Dim> const &point){

				for(size_t i=0; i<Dim; ++i){
					if( point[i] < training_range[i][0] || point[i] > training_range[i][1] )
						return false;
				}
				return true;
			}


			node<Dim, Num_Variants, Num_Vertices, MeasureType> *search(std::array<size_t, Dim> const &point){

				if( contains(point) ){
					if( children.empty() )
						return this;
					else{
						//make each children to search
						for(auto &i:children){
							auto result=i.search(point);
							if(result!=NULL)
								return result;
						}
						//std::cout<<"Sth is wrong, not find leaf as children"<<std::endl;
					}


				}

				return NULL;

			}


			size_t nearest_neighbor(std::array<size_t, Dim> const &point){

				std::array<size_t, Dim> distances;

				std::transform(point.begin(), point.end(), vertices[0].tunable_args.begin(), distances.begin(),
						[](size_t const x1, size_t const x2){ return (x1-x2)*(x1-x2); }
					      );

				size_t lowest_so_far = std::accumulate(distances.begin(), distances.end(), 0), now;
				size_t lowest_index = 0;

				for(size_t i=1; i<Num_Vertices;++i){

					std::transform(point.begin(), point.end(), vertices[i].tunable_args.begin(), distances.begin(),
							[](size_t const x1, size_t const x2){ return (x1-x2)*(x1-x2); }
						      );

					now=std::accumulate(distances.begin(), distances.end(), 0);
					if(now<lowest_so_far){
						lowest_so_far=now;
						lowest_index=i;
					}

				}



				return vertices[lowest_index].winners[0];
			}



		};

	template <size_t Dim, size_t Num_Variants, size_t Num_Vertices, class MeasureType, class ...Tunable_Args>
		struct tree {


			explicit tree(tuneit_settings<Dim, Num_Variants> const &_settings): settings(_settings), depth(0), root(_settings){}

			tuneit_settings<Dim, Num_Variants> const &settings;
			size_t depth;
			node<Dim, Num_Variants, Num_Vertices, MeasureType> root;

			void print_dot(){
				/*{{{*/
				std::ofstream file("tree_depth_"+std::to_string(depth)+".dot");

				size_t next_available_id=0 ;

				//print header
				file<<"digraph G"<<std::endl;
				file<<"{"<<std::endl;
				file<<"\tnode [shape = none];"<<std::endl;
				file<<"\trankdir = LR;"<<std::endl;

				//non-recursive call
				root.print_one_node(file, 0);

				for(auto it=root.children.begin(); it!=root.children.end(); ++it){
					++next_available_id;
					//recursive call
					it->print_one_node_with_link(file, 0, next_available_id, next_available_id);
				}



				//print closing node
				file<<"}"<<std::endl;

				file.close();

				//system("dot -Tsvg tree.dot > tree.svg");
				//system("eog tree.svg");
				/*}}}*/
			}

			node<Dim, Num_Variants,  Num_Vertices, MeasureType> *search_leaf(std::array<size_t, Dim> const &point){

				return root.search(point);



			}





		};
	/*}}}*/

	//interface: tuner
	/*{{{*/


	template <size_t Num_Variants, size_t Num_Vertices, class Tunable, class MeasureType, class ...Tunable_Args>
		struct tuneit{
			explicit tuneit(tuneit_settings<sizeof...(Tunable_Args), Num_Variants> const &_settings): settings(_settings), kd_tree(_settings){}


			tuneit_settings<sizeof...(Tunable_Args), Num_Variants> const &settings;
			tree<sizeof...(Tunable_Args), Num_Variants, Num_Vertices, MeasureType, Tunable_Args...> kd_tree;
			Tunable const impls;

			//Tunable impls;

			void train(){

#ifdef SCIENTIFIC_EXPERIMENT
				sampled_count=0;
#endif

				//tree -> vector of training points -> application's tunable

				//root can start 2, but other nodes go to step 1 by step 5
				//for each depth, do the following
				//five states:
				//1) init with range, add the node itself to worklist
				//2) grow vertices
				//3) measure each vertices
				//4) do 3) for all nodes in worklist
				//5) if not closed, grow children and go to 1)

				//each time one level is done,
				//all the vertices should be already measured

				std::vector<node<sizeof...(Tunable_Args), Num_Variants,  Num_Vertices, MeasureType> *> worklist;

				//for root
				//std::cerr<<"before grow..."<<std::endl;
				kd_tree.root.grow_vertices();
				//std::cerr<<"before measure..."<<std::endl;
				kd_tree.root.measure(impls, NULL);
				//std::cerr<<"before update_closeness..."<<std::endl;
				kd_tree.root.update_closeness();
				if(!kd_tree.root.closedness){
					worklist.push_back(&kd_tree.root);
				}
				++kd_tree.depth;

				decltype(worklist) new_worklist;


				while(kd_tree.depth<settings.depth && !worklist.empty() ){


					//make the space ready for the worst case
					new_worklist.reserve( pow(Num_Vertices, kd_tree.depth) );


					for(auto iter=worklist.begin(); iter!=worklist.end(); ++iter){
						(*iter)->grow_children();

						for(auto iter2=(*iter)->children.begin(); iter2!=(*iter)->children.end(); ++iter2){
							iter2->grow_vertices();
							iter2->measure(impls, (*iter));
							iter2->update_closeness();
							if(!iter2->closedness)
								new_worklist.push_back( &(*iter2) );
						}
					}

					worklist=std::move(new_worklist);
					new_worklist.clear();

					++kd_tree.depth;



				}

				kd_tree.print_dot();


			}
			size_t predict(Tunable_Args const ... tunable_args){

				//now the problem is how to convert tunable_args
				//to an array
				//
				//this is super cool
				std::array<size_t, sizeof...(Tunable_Args)> const point{tunable_args...};

				//traverse to leaf
				auto wanted_leaf=kd_tree.search_leaf(point);
				assert(wanted_leaf != NULL);
				//if closed, return anyone
				if(wanted_leaf->closedness)
					return wanted_leaf->vertices[0].winners[0];
				//if open, return nearest neighbor
				else
					return wanted_leaf->nearest_neighbor(point);

			}

			template <class ... All_Args>
			void run(size_t predicted_index, All_Args& ...all_args ){
				//first predict
				impls.run(predicted_index, all_args...);
			}

#ifdef SCIENTIFIC_EXPERIMENT
			size_t sampled_points_count(){return sampled_count;}
#endif


			void load(std::string filename){}
			void save(std::string filename){}


		};
	/*}}}*/

}



#endif
