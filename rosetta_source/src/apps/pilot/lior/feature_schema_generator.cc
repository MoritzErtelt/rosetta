#include <iostream>
#include <protocols/features/FeaturesReporterFactory.hh>
#include <protocols/features/FeaturesReporter.hh>
#include <core/init.hh>
#include <vector>
#include <string>
#include <protocols/init/init.hh>
int
main( int argc, char * argv [] )
{
	using namespace protocols;
	using namespace protocols::features;
	protocols::init::init(argc,argv);
    protocols::features::FeaturesReporterFactory *factory = FeaturesReporterFactory::get_instance();
    utility::vector1<std::string> all_features = factory->get_all_features_names();
    for (int i=1; i<=all_features.size(); ++i) {
    	FeaturesReporterOP feature_reporter = factory->get_features_reporter(all_features[i]);
    	std::string schema = feature_reporter->schema();
    	std::cout<<schema<<std::endl;
    }
    return 0;

}
