TEMPLATE = aux

HEADERS += \
    rpp.hpp \
    rpp_utils.hpp \
    rpp_emitter.hpp \
    rpp_test.hpp \
    rpp_concepts.hpp \
    rpp_property.hpp

headers.path = $$PREFIX/include
headers.files = $$HEADERS

INSTALLS += headers
