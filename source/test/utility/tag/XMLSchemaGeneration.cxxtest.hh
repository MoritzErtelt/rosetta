// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
// :noTabs=false:tabSize=4:indentSize=4:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   test/utility/tag/XMLSchemaGeneration.cxxtest.hh
/// @brief  test suite for the XMLSchemaGeneration classes
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

// Test headers
#include <cxxtest/TestSuite.h>
#include <test/core/init_util.hh>

// Unit headers
#include <utility/tag/XMLSchemaGeneration.hh>

// Utility headers
#include <utility/excn/Exceptions.hh>

// C++ headers
#include <sstream>

using namespace utility::tag;

/// @brief Little utility function for identifying the first position at which two strings
/// differ.  Returns 1 greater than the shorter of the two strings if there is no difference
/// until the first string ends -- compare what is returned against the max of the two strings'
/// sizes() to tell if both strings are equal.
platform::Size
first_difference( std::string const & s1, std::string const & s2 ) {
	for ( platform::Size ii = 0; ii < std::min( s1.size(), s2.size() ); ++ii ) {
		if ( s1[ii] != s2[ii] ) { return ii; }
	}
	return std::min( s1.size(), s2.size() );
}

class XMLSchemaGenerationTests : public CxxTest::TestSuite
{
public:

	void test_xmlschema_data_type() {
		XMLSchemaType xsstr; xsstr.type( xs_string );
		TS_ASSERT_EQUALS( std::string( "xs:string" ), xsstr.type_name() );

		XMLSchemaType xsdec; xsdec.type( xs_decimal );
		TS_ASSERT_EQUALS( std::string( "xs:decimal" ), xsdec.type_name() );

		XMLSchemaType xsint; xsint.type( xs_integer );
		TS_ASSERT_EQUALS( std::string( "xs:integer" ), xsint.type_name() );

		XMLSchemaType xsbool; xsbool.type( xs_boolean );
		TS_ASSERT_EQUALS( std::string( "xs:boolean" ), xsbool.type_name() );

		XMLSchemaType xsdate; xsdate.type( xs_date );
		TS_ASSERT_EQUALS( std::string( "xs:date" ), xsdate.type_name() );

		XMLSchemaType xstime; xstime.type( xs_time );
		TS_ASSERT_EQUALS( std::string( "xs:time" ), xstime.type_name() );

		XMLSchemaType cs_intlist;
		cs_intlist.custom_type_name( "int_cslist" );
		TS_ASSERT_EQUALS( std::string( "int_cslist" ), cs_intlist.type_name() );
	}

	void test_xs_attribute_custom() {
		XMLSchemaAttribute att;
		att.name( "residues" );
		XMLSchemaType cs_intlist;
		cs_intlist.custom_type_name( "int_cslist" );
		att.type( cs_intlist );
		std::ostringstream oss;
		att.write_definition( 0, oss );
		TS_ASSERT_EQUALS( std::string( "<xs:attribute name=\"residues\" type=\"int_cslist\"/>\n" ), oss.str() );
	}

	void test_xs_attribute_string() {
		XMLSchemaAttribute att;
		att.name( "chain" );
		XMLSchemaType xsstr( xs_string );
		att.type( xsstr );
		std::ostringstream oss;
		att.write_definition( 0, oss );
		TS_ASSERT_EQUALS( std::string( "<xs:attribute name=\"chain\" type=\"xs:string\"/>\n" ), oss.str() );
	}

	void test_xs_attribute_string_required() {
		XMLSchemaAttribute att;
		att.name( "chain" );
		XMLSchemaType xsstr( xs_string );
		att.type( xsstr );
		att.is_required( true );
		std::ostringstream oss;
		att.write_definition( 0, oss );
		TS_ASSERT_EQUALS( std::string( "<xs:attribute name=\"chain\" type=\"xs:string\" use=\"required\"/>\n" ), oss.str() );
	}

	void test_xs_attribute_int_default() {
		XMLSchemaAttribute att;
		att.name( "jump" );
		att.type( XMLSchemaType( xs_integer ) );
		att.default_value( "1" );
		std::ostringstream oss;
		att.write_definition( 0, oss );
		TS_ASSERT_EQUALS( std::string( "<xs:attribute name=\"jump\" type=\"xs:integer\" default=\"1\"/>\n" ), oss.str() );
	}

	void test_xmlschema_restriction_int_range() {
		XMLSchemaRestriction int_1_to_100;
		int_1_to_100.name( "grade" );
		int_1_to_100.base_type( XMLSchemaType( xs_integer ) );
		int_1_to_100.add_restriction( xsr_minInclusive, "1" );
		int_1_to_100.add_restriction( xsr_maxInclusive, "100" );
		std::string definition = "<xs:simpleType name=\"grade\">\n <xs:restriction base=\"xs:integer\">\n  <xs:minInclusive value=\"1\"/>\n"
			"  <xs:maxInclusive value=\"100\"/>\n </xs:restriction>\n</xs:simpleType>\n";
		std::ostringstream oss;
		int_1_to_100.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );

		std::string definition_indent2 = "  <xs:simpleType name=\"grade\">\n   <xs:restriction base=\"xs:integer\">\n    <xs:minInclusive value=\"1\"/>\n"
			"    <xs:maxInclusive value=\"100\"/>\n   </xs:restriction>\n  </xs:simpleType>\n";
		std::ostringstream oss2;
		int_1_to_100.write_definition( 2, oss2 );
		TS_ASSERT_EQUALS( definition_indent2, oss2.str() );

	}

	void test_xmlschema_restriction_enumeration() {
		XMLSchemaRestriction enumeration;
		enumeration.name( "color_channel" );
		enumeration.base_type( XMLSchemaType( xs_string ) );
		enumeration.add_restriction( xsr_enumeration, "red" );
		enumeration.add_restriction( xsr_enumeration, "green" );
		enumeration.add_restriction( xsr_enumeration, "blue" );
		std::string definition = "<xs:simpleType name=\"color_channel\">\n <xs:restriction base=\"xs:string\">\n  <xs:enumeration value=\"red\"/>\n"
			"  <xs:enumeration value=\"green\"/>\n  <xs:enumeration value=\"blue\"/>\n </xs:restriction>\n</xs:simpleType>\n";
		std::ostringstream oss;
		enumeration.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xmlschema_restriction_pattern() {
		XMLSchemaRestriction comma_separated_int_list;
		comma_separated_int_list.name( "int_cslist" );
		comma_separated_int_list.base_type( XMLSchemaType( xs_string ) );
		comma_separated_int_list.add_restriction( xsr_pattern, "[0-9]+(,[0-9]+)*" );
		std::string definition = "<xs:simpleType name=\"int_cslist\">\n <xs:restriction base=\"xs:string\">\n  <xs:pattern value=\"[0-9]+(,[0-9]+)*\"/>\n"
			" </xs:restriction>\n</xs:simpleType>\n";
		std::ostringstream oss;
		comma_separated_int_list.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xmlschema_restriction_type_names() {
		TS_ASSERT_EQUALS( restriction_type_name( xsr_enumeration ), "xs:enumeration" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_fractionDigits ), "xs:fractionDigits" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_length ), "xs:length" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_maxExclusive ), "xs:maxExclusive" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_maxInclusive ), "xs:maxInclusive" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_maxLength ), "xs:maxLength" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_minExclusive ), "xs:minExclusive" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_minInclusive ), "xs:minInclusive" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_minLength ), "xs:minLength" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_pattern ), "xs:pattern" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_totalDigits ), "xs:totalDigits" );
		TS_ASSERT_EQUALS( restriction_type_name( xsr_whitespace ), "xs:whitespace" );
	}

	void test_xml_complex_type_sequence1() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_one" );
		ct.type( xsctt_sequence );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		std::string definition = "<xs:complexType name=\"seq_of_one\" mixed=\"true\">\n <xs:sequence>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n </xs:sequence>\n</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_complex_type_sequence2() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_two" );
		ct.type( xsctt_sequence );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		ct.add_subelement( elem2 );
		std::string definition = "<xs:complexType name=\"seq_of_two\" mixed=\"true\">\n <xs:sequence>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n"
			" </xs:sequence>\n</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_complex_type_sequence_w_attributes() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_two" );
		ct.type( xsctt_sequence );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		ct.add_subelement( elem2 );
		ct.add_attribute( XMLSchemaAttribute( "attr1", xs_string, "1" )); // default value constructor
		ct.add_attribute( XMLSchemaAttribute( "attr2", xs_boolean, true )); // is_required constructor

		std::string definition = "<xs:complexType name=\"seq_of_two\" mixed=\"true\">\n <xs:sequence>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n"
			" </xs:sequence>\n"
			" <xs:attribute name=\"attr1\" type=\"xs:string\" default=\"1\"/>\n"
			" <xs:attribute name=\"attr2\" type=\"xs:boolean\" use=\"required\"/>\n"
			"</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_complex_type_empty_w_attributes() {
		XMLSchemaComplexType ct;
		ct.name( "empty_w_two_attrs" );
		ct.type( xsctt_empty );
		ct.add_attribute( XMLSchemaAttribute( "attr1", xs_string, "1" )); // default value constructor
		ct.add_attribute( XMLSchemaAttribute( "attr2", xs_boolean, true )); // is_required constructor

		std::string definition = "<xs:complexType name=\"empty_w_two_attrs\" mixed=\"true\">\n"
			" <xs:attribute name=\"attr1\" type=\"xs:string\" default=\"1\"/>\n"
			" <xs:attribute name=\"attr2\" type=\"xs:boolean\" use=\"required\"/>\n"
			"</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	/// @brief make sure that "xs:sequence" doesn't get written out if there are no sub-elements
	void test_xml_complex_type_empty_but_seq_w_attributes() {
		XMLSchemaComplexType ct;
		ct.name( "empty_w_two_attrs" );
		ct.type( xsctt_sequence );
		ct.add_attribute( XMLSchemaAttribute( "attr1", xs_string, "1" )); // default value constructor
		ct.add_attribute( XMLSchemaAttribute( "attr2", xs_boolean, true )); // is_required constructor

		std::string definition = "<xs:complexType name=\"empty_w_two_attrs\" mixed=\"true\">\n"
			" <xs:attribute name=\"attr1\" type=\"xs:string\" default=\"1\"/>\n"
			" <xs:attribute name=\"attr2\" type=\"xs:boolean\" use=\"required\"/>\n"
			"</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	// choice
	void test_xml_complex_type_choice1() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_one" );
		ct.type( xsctt_choice );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		std::string definition = "<xs:complexType name=\"seq_of_one\" mixed=\"true\">\n <xs:choice>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n </xs:choice>\n</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_complex_type_choice2() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_two" );
		ct.type( xsctt_choice );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		ct.add_subelement( elem2 );
		std::string definition = "<xs:complexType name=\"seq_of_two\" mixed=\"true\">\n <xs:choice>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n"
			" </xs:choice>\n</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_complex_type_choice_w_attributes() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_two" );
		ct.type( xsctt_choice );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		ct.add_subelement( elem2 );
		ct.add_attribute( XMLSchemaAttribute( "attr1", xs_string, "1" )); // default value constructor
		ct.add_attribute( XMLSchemaAttribute( "attr2", xs_boolean, true )); // is_required constructor

		std::string definition = "<xs:complexType name=\"seq_of_two\" mixed=\"true\">\n <xs:choice>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n"
			" </xs:choice>\n"
			" <xs:attribute name=\"attr1\" type=\"xs:string\" default=\"1\"/>\n"
			" <xs:attribute name=\"attr2\" type=\"xs:boolean\" use=\"required\"/>\n"
			"</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	// all
	void test_xml_complex_type_all1() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_one" );
		ct.type( xsctt_all );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		std::string definition = "<xs:complexType name=\"seq_of_one\" mixed=\"true\">\n <xs:all>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n </xs:all>\n</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_complex_type_all2() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_two" );
		ct.type( xsctt_all );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		ct.add_subelement( elem2 );
		std::string definition = "<xs:complexType name=\"seq_of_two\" mixed=\"true\">\n <xs:all>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n"
			" </xs:all>\n</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_complex_type_all_w_attributes() {
		XMLSchemaComplexType ct;
		ct.name( "seq_of_two" );
		ct.type( xsctt_all );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		ct.add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		ct.add_subelement( elem2 );
		ct.add_attribute( XMLSchemaAttribute( "attr1", xs_string, "1" )); // default value constructor
		ct.add_attribute( XMLSchemaAttribute( "attr2", xs_boolean, true )); // is_required constructor

		std::string definition = "<xs:complexType name=\"seq_of_two\" mixed=\"true\">\n <xs:all>\n"
			"  <xs:element name=\"elem1\" type=\"xs:string\"/>\n  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n"
			" </xs:all>\n"
			" <xs:attribute name=\"attr1\" type=\"xs:string\" default=\"1\"/>\n"
			" <xs:attribute name=\"attr2\" type=\"xs:boolean\" use=\"required\"/>\n"
			"</xs:complexType>\n";
		std::ostringstream oss;
		ct.write_definition( 0, oss );
		TS_ASSERT_EQUALS( definition, oss.str() );
	}


	void test_complex_type_group_sequence() {
		XMLSchemaComplexTypeOP seqtype( new XMLSchemaComplexType );
		seqtype->type( xsctt_sequence );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		seqtype->add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		seqtype->add_subelement( elem2 );

		XMLSchemaComplexType grouptype;
		grouptype.name( "group_seq" );
		grouptype.subtype( seqtype );

		std::ostringstream oss;
		grouptype.write_definition( 0, oss );

		std::string definition = "<xs:group name=\"group_seq\">\n <xs:sequence>\n  <xs:element name=\"elem1\" type=\"xs:string\"/>\n"
			"  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n </xs:sequence>\n</xs:group>\n";
		TS_ASSERT_EQUALS( definition, oss.str() );

	}

	void test_complex_type_group_choice() {
		XMLSchemaComplexTypeOP seqtype( new XMLSchemaComplexType );
		seqtype->type( xsctt_choice );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		seqtype->add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		seqtype->add_subelement( elem2 );

		XMLSchemaComplexType grouptype;
		grouptype.name( "group_seq" );
		grouptype.subtype( seqtype );

		std::ostringstream oss;
		grouptype.write_definition( 0, oss );

		std::string definition = "<xs:group name=\"group_seq\">\n <xs:choice>\n  <xs:element name=\"elem1\" type=\"xs:string\"/>\n"
			"  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n </xs:choice>\n</xs:group>\n";
		TS_ASSERT_EQUALS( definition, oss.str() );

	}

	void test_complex_type_group_all() {
		XMLSchemaComplexTypeOP seqtype( new XMLSchemaComplexType );
		seqtype->type( xsctt_all );
		XMLSchemaElementOP elem1( new XMLSchemaElement ); elem1->name( "elem1" ); elem1->type_name( xs_string );
		seqtype->add_subelement( elem1 );
		XMLSchemaElementOP elem2( new XMLSchemaElement ); elem2->name( "elem2" ); elem2->type_name( xs_integer );
		seqtype->add_subelement( elem2 );

		XMLSchemaComplexType grouptype;
		grouptype.name( "group_seq" );
		grouptype.subtype( seqtype );

		std::ostringstream oss;
		grouptype.write_definition( 0, oss );

		std::string definition = "<xs:group name=\"group_seq\">\n <xs:all>\n  <xs:element name=\"elem1\" type=\"xs:string\"/>\n"
			"  <xs:element name=\"elem2\" type=\"xs:integer\"/>\n </xs:all>\n</xs:group>\n";
		TS_ASSERT_EQUALS( definition, oss.str() );

	}

	void test_xml_element_type_reference() {
		XMLSchemaElement reslist;
		reslist.name( "residues" );
		reslist.type_name( "int_cslist" );
		std::ostringstream oss;
		reslist.write_definition( 2, oss );
		std::string definition = "  <xs:element name=\"residues\" type=\"int_cslist\"/>\n";
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_element_type_reference_w_occurs() {
		XMLSchemaElement reslist;
		reslist.name( "residues" );
		reslist.type_name( "int_cslist" );
		reslist.min_occurs( 3 );
		reslist.max_occurs( 10 );
		std::ostringstream oss;
		reslist.write_definition( 2, oss );
		std::string definition = "  <xs:element name=\"residues\" type=\"int_cslist\" minOccurs=\"3\" maxOccurs=\"10\"/>\n";
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_element_group_reference() {
		XMLSchemaElement selectors;
		selectors.group_name( "residue_selector" );
		std::ostringstream oss;
		selectors.write_definition( 2, oss );
		std::string definition = "  <xs:group ref=\"residue_selector\"/>\n";
		TS_ASSERT_EQUALS( definition, oss.str() );
	}


	void test_xml_element_group_reference_w_occurs() {
		XMLSchemaElement selectors;
		selectors.group_name( "residue_selector" );
		selectors.min_occurs( 2 );
		selectors.max_occurs( xsminmax_unbounded );
		std::ostringstream oss;
		selectors.write_definition( 2, oss );
		std::string definition = "  <xs:group ref=\"residue_selector\" minOccurs=\"2\" maxOccurs=\"unbounded\"/>\n";
		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_element_w_complex_type() {
		XMLSchemaElement and_selector;
		and_selector.name( "AndSelector" );

		XMLSchemaElementOP selectors( new XMLSchemaElement );
		selectors->group_name( "residue_selector" );
		selectors->min_occurs( 2 );
		selectors->max_occurs( xsminmax_unbounded );

		XMLSchemaComplexTypeOP and_type( new XMLSchemaComplexType );
		and_type->type( xsctt_choice );
		and_type->add_subelement( selectors );
		and_type->add_attribute( XMLSchemaAttribute( "some_attribute", xs_string ));

		and_selector.element_type_def( and_type );

		std::ostringstream oss;
		and_selector.write_definition( 0, oss );
		std::string definition = "<xs:element name=\"AndSelector\">\n <xs:complexType mixed=\"true\">\n  <xs:choice>\n"
			"   <xs:group ref=\"residue_selector\" minOccurs=\"2\" maxOccurs=\"unbounded\"/>\n"
			"  </xs:choice>\n  <xs:attribute name=\"some_attribute\" type=\"xs:string\"/>\n"
			" </xs:complexType>\n</xs:element>\n";

		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_schema_element_w_restriction_type() {
		XMLSchemaElement element;
		element.name( "residues" );
		XMLSchemaRestrictionOP cs_intlist( new XMLSchemaRestriction );
		cs_intlist->base_type( xs_string );
		cs_intlist->add_restriction( xsr_pattern, "[0-9]+(,[0-9]+)*" );
		element.restriction_type_def( cs_intlist );

		std::ostringstream oss;
		element.write_definition( 0, oss );

		std::string definition = "<xs:element name=\"residues\">\n <xs:simpleType>\n  <xs:restriction base=\"xs:string\">\n"
			"   <xs:pattern value=\"[0-9]+(,[0-9]+)*\"/>\n"
			"  </xs:restriction>\n </xs:simpleType>\n</xs:element>\n";

		TS_ASSERT_EQUALS( definition, oss.str() );
	}

	void test_xml_schema_definition_detect_name_collision() {
		XMLSchemaDefinition xsd;
		xsd.add_top_level_element( "RosettaScripts", "<xs:element name=\"RosettaScripts\" type=\"xs:string\"/>\n" );
		try {
			xsd.add_top_level_element( "RosettaScripts", "<xs:element name=\"RosettaScripts\" type=\"xs:integer\"/>\n" );
			TS_ASSERT( false ); // we should not have reached this point
		} catch ( utility::excn::EXCN_Msg_Exception const & e ) {
			TS_ASSERT_EQUALS( e.msg(), "Name collision in creation of XML Schema definition: top level element with name \"RosettaScripts\" already has been defined.\n"
				"Old definition:\n<xs:element name=\"RosettaScripts\" type=\"xs:string\"/>\nNew definition:\n"
				"<xs:element name=\"RosettaScripts\" type=\"xs:integer\"/>\n");
		}
	}

	void test_xml_schema_definition_detect_name_mismatch() {
		XMLSchemaDefinition xsd;
		try {
			xsd.add_top_level_element( "imposter", "<xs:element name=\"RosettaScripts\" type=\"xs:integer\"/>\n" );
		} catch ( utility::excn::EXCN_Msg_Exception const & e ) {
			TS_ASSERT_EQUALS( e.msg(), "top level tag with presumed name \"imposter\" has an actual name of \"RosettaScripts\"" );
		}
	}

	void test_xml_schema_definition_detect_missing_name() {
		XMLSchemaDefinition xsd;
		try {
			xsd.add_top_level_element( "RosettaScripts", "<xs:element type=\"xs:integer\"/>\n" );
		} catch ( utility::excn::EXCN_Msg_Exception const & e ) {
			TS_ASSERT_EQUALS( e.msg(), "top level tag \"RosettaScripts\" does not have a name attribute (a.k.a. option)." );
		}
	}

	void test_xml_schema_definition_write_xsd() {
		XMLSchemaDefinition xsd;

		xsd.add_top_level_element(
			"ResidueSelectorType",
			"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
			" <xs:group ref=\"ResidueSelector\"/>\n"
			"</xs:complexType>\n"
		);

		xsd.add_top_level_element(
			"ResidueSelector",
			"<xs:group name=\"ResidueSelector\">\n"
			" <xs:choice>\n"
			"  <xs:element name=\"AndResidueSelector\" type=\"AndResidueSelectorType\"/>\n"
			"  <xs:element name=\"OrResidueSelector\" type=\"OrResidueSelectorType\"/>\n"
			"  <xs:element name=\"NotResidueSelector\" type=\"NotResidueSelectorType\"/>\n"
			"  <xs:element name=\"ChainResidueSelector\" type=\"ChainResidueSelectorType\"/>\n"
			"  <xs:element name=\"NeighborhoodResidueSelector\" type=\"NeighborhoodResidueSelectorType\"/>\n"
			" </xs:choice>\n"
			"</xs:group>\n"
		);

		std::string xsd_expected_output =
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
			"\n"
			"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
			" <xs:group ref=\"ResidueSelector\"/>\n"
			"</xs:complexType>\n"
			"\n"
			"<xs:group name=\"ResidueSelector\">\n"
			" <xs:choice>\n"
			"  <xs:element name=\"AndResidueSelector\" type=\"AndResidueSelectorType\"/>\n"
			"  <xs:element name=\"OrResidueSelector\" type=\"OrResidueSelectorType\"/>\n"
			"  <xs:element name=\"NotResidueSelector\" type=\"NotResidueSelectorType\"/>\n"
			"  <xs:element name=\"ChainResidueSelector\" type=\"ChainResidueSelectorType\"/>\n"
			"  <xs:element name=\"NeighborhoodResidueSelector\" type=\"NeighborhoodResidueSelectorType\"/>\n"
			" </xs:choice>\n"
			"</xs:group>\n"
			"\n"
			"</xs:schema>\n";

		//std::cout << xsd_expected_output << "\nand\n" << xsd.full_definition() << std::endl;


		TS_ASSERT_EQUALS( xsd.full_definition(), xsd_expected_output );

	}

	void test_xml_schema_definition_write_xsd_w_identical_duplicate() {
		XMLSchemaDefinition xsd;

		xsd.add_top_level_element(
			"ResidueSelectorType",
			"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
			" <xs:group ref=\"ResidueSelector\"/>\n"
			"</xs:complexType>\n"
		);

		xsd.add_top_level_element(
			"ResidueSelector",
			"<xs:group name=\"ResidueSelector\">\n"
			" <xs:choice>\n"
			"  <xs:element name=\"AndResidueSelector\" type=\"AndResidueSelectorType\"/>\n"
			"  <xs:element name=\"OrResidueSelector\" type=\"OrResidueSelectorType\"/>\n"
			"  <xs:element name=\"NotResidueSelector\" type=\"NotResidueSelectorType\"/>\n"
			"  <xs:element name=\"ChainResidueSelector\" type=\"ChainResidueSelectorType\"/>\n"
			"  <xs:element name=\"NeighborhoodResidueSelector\" type=\"NeighborhoodResidueSelectorType\"/>\n"
			" </xs:choice>\n"
			"</xs:group>\n"
		);

		// now for the duplicated element
		xsd.add_top_level_element(
			"ResidueSelectorType",
			"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
			" <xs:group ref=\"ResidueSelector\"/>\n"
			"</xs:complexType>\n"
		);

		std::string xsd_expected_output =
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
			"\n"
			"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
			" <xs:group ref=\"ResidueSelector\"/>\n"
			"</xs:complexType>\n"
			"\n"
			"<xs:group name=\"ResidueSelector\">\n"
			" <xs:choice>\n"
			"  <xs:element name=\"AndResidueSelector\" type=\"AndResidueSelectorType\"/>\n"
			"  <xs:element name=\"OrResidueSelector\" type=\"OrResidueSelectorType\"/>\n"
			"  <xs:element name=\"NotResidueSelector\" type=\"NotResidueSelectorType\"/>\n"
			"  <xs:element name=\"ChainResidueSelector\" type=\"ChainResidueSelectorType\"/>\n"
			"  <xs:element name=\"NeighborhoodResidueSelector\" type=\"NeighborhoodResidueSelectorType\"/>\n"
			" </xs:choice>\n"
			"</xs:group>\n"
			"\n"
			"</xs:schema>\n";

		//std::cout << xsd_expected_output << "\nand\n" << xsd.full_definition() << std::endl;


		TS_ASSERT_EQUALS( xsd.full_definition(), xsd_expected_output );

	}

	void test_xml_schema_definition_write_xsd_w_nonidentical_duplicate() {
		XMLSchemaDefinition xsd;

		xsd.add_top_level_element(
			"ResidueSelectorType",
			"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
			" <xs:group ref=\"ResidueSelector\"/>\n"
			"</xs:complexType>\n"
		);

		xsd.add_top_level_element(
			"ResidueSelector",
			"<xs:group name=\"ResidueSelector\">\n"
			" <xs:choice>\n"
			"  <xs:element name=\"AndResidueSelector\" type=\"AndResidueSelectorType\"/>\n"
			"  <xs:element name=\"OrResidueSelector\" type=\"OrResidueSelectorType\"/>\n"
			"  <xs:element name=\"NotResidueSelector\" type=\"NotResidueSelectorType\"/>\n"
			"  <xs:element name=\"ChainResidueSelector\" type=\"ChainResidueSelectorType\"/>\n"
			"  <xs:element name=\"NeighborhoodResidueSelector\" type=\"NeighborhoodResidueSelectorType\"/>\n"
			" </xs:choice>\n"
			"</xs:group>\n"
		);

		try {

			// now for the duplicated element
			xsd.add_top_level_element(
				"ResidueSelectorType",
				"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
				" <xs:group ref=\"TResidueSelector\"/>\n"
				"</xs:complexType>\n"
			);
			TS_ASSERT( false ); // this should not be reached
		} catch ( utility::excn::EXCN_Msg_Exception const & e ) {
			std::string expected_message = "Name collision in creation of XML Schema definition: top level element with "
				"name \"ResidueSelectorType\" already has been defined.\n"
				"Old definition:\n"
				"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
				" <xs:group ref=\"ResidueSelector\"/>\n"
				"</xs:complexType>\n"
				"New definition:\n"
				"<xs:complexType name=\"ResidueSelectorType\" mixed=\"true\">\n"
				" <xs:group ref=\"TResidueSelector\"/>\n"
				"</xs:complexType>\n";
			TS_ASSERT_EQUALS( e.msg(), expected_message );
			// std::cout << std::endl << std::endl;
			// std::cout << e.msg() << std::endl << std::endl;
			// std::cout << expected_message << std::endl << std::endl;
			// platform::Size first_diff = first_difference( e.msg(), expected_message );
			// if ( first_diff < e.msg().size() && first_diff < expected_message.size() ) {
			// 	std::cout << "first difference between messages: pos " << first_diff+1 << " '" << e.msg()[ first_diff ] << "' vs '"
			// 						<< expected_message[ first_diff ] << "'" << std::endl;
			// } else {
			// 	std::cout << "first diff pos " << first_diff << " vs sizes " << e.msg().size() << " and " << expected_message.size() << std::endl;
			// }
		}
	}


};