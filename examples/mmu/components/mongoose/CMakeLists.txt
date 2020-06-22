set(mon_srcs "mongoose.c")

idf_component_register(SRCS "${mon_srcs}"
                       INCLUDE_DIRS "."
                      PRIV_REQUIRES)