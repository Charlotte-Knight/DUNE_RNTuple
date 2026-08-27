#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace classschema;
#pragma link C++ class classschema::V3+;
#pragma link C++ enum classschema::PartEMethod;
#pragma link C++ enum classschema::RecoObjType;
#pragma link C++ class classschema::Particle+;
#pragma link C++ class classschema::Interaction+;
#pragma link C++ class classschema::Common+;
#pragma link C++ class classschema::TruthParticle+;
#pragma link C++ class classschema::TruthInteraction+;
#pragma link C++ class classschema::Mc+;

#pragma link C++ class std::vector<classschema::Particle>+;
#pragma link C++ class std::vector<classschema::Interaction>+;
#pragma link C++ class std::vector<classschema::TruthParticle>+;
#pragma link C++ class std::vector<classschema::TruthInteraction>+;

#endif
