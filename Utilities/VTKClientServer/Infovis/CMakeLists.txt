SET(KIT_WRAP_DEPS Filtering Rendering Parallel)
IF(VTK_USE_PARALLEL)
   SET(KIT_WRAP_DEPS ${KIT_WRAP_DEPS} Parallel)
 ENDIF(VTK_USE_PARALLEL)
PV_WRAP_VTK_CS(Infovis INFOVIS "${KIT_WRAP_DEPS}")
