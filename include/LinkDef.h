#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ enum PartEMethod;
#pragma link C++ enum RecoObjType;
#pragma link C++ class Particle+;
#pragma link C++ class Interaction+;
#pragma link C++ class common+;
#pragma link C++ class TruthParticle+;
#pragma link C++ class TruthInteraction+;
#pragma link C++ class mc+;

#pragma link C++ class std::vector<Particle>+;
#pragma link C++ class std::vector<Interaction>+;
#pragma link C++ class std::vector<common>+;
#pragma link C++ class std::vector<TruthParticle>+;
#pragma link C++ class std::vector<TruthInteraction>+;
#pragma link C++ class std::vector<mc>+;

// #pragma read                                  \
//   sourceClass="Particle"                        \
//   source="float E" \
//   checksum="[0x9878e129]"                   \
//   targetClass="Particle"                        \
//   target="Enu"                                \
//   embed="true"                                \
//   include="iostream,cstdlib"                  \
//   code="{ Enu = onfile.E; }"

// #pragma read                                  \
//   sourceClass="TruthParticle"                   \
//   source="float E" \
//   checksum="[0xc270898c]"                   \
//   targetClass="TruthParticle"                   \
//   target="Enu"                                \
//   embed="true"                                \
//   include="iostream,cstdlib"                  \
//   code="{ Enu = onfile.E; }"

#endif
