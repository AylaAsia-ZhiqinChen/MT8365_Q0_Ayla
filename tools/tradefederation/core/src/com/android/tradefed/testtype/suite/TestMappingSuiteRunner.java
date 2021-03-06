/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.tradefed.testtype.suite;

import com.android.tradefed.config.ConfigurationDescriptor;
import com.android.tradefed.config.IConfiguration;
import com.android.tradefed.config.Option;
import com.android.tradefed.util.testmapping.TestInfo;
import com.android.tradefed.util.testmapping.TestMapping;
import com.android.tradefed.util.testmapping.TestOption;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
/**
 * Implementation of {@link BaseTestSuite} to run tests specified by option include-filter, or
 * TEST_MAPPING files from build, as a suite.
 */
public class TestMappingSuiteRunner extends BaseTestSuite {

    @Option(
        name = "test-mapping-test-group",
        description =
                "Group of tests to run, e.g., presubmit, postsubmit. The suite runner "
                        + "shall load the tests defined in all TEST_MAPPING files in the source "
                        + "code, through build artifact test_mappings.zip."
    )
    private String mTestGroup = null;

    @Option(
        name = "test-mapping-keyword",
        description =
                "Keyword to be matched to the `keywords` setting of a test configured in "
                        + "a TEST_MAPPING file. The test will only run if it has all the keywords "
                        + "specified in the option. If option test-mapping-test-group is not set, "
                        + "test-mapping-keyword option is ignored as the tests to run are not "
                        + "loaded directly from TEST_MAPPING files but is supplied via the "
                        + "--include-filter arg."
    )
    private Set<String> mKeywords = new HashSet<>();

    /** Special definition in the test mapping structure. */
    private static final String TEST_MAPPING_INCLUDE_FILTER = "include-filter";

    private static final String TEST_MAPPING_EXCLUDE_FILTER = "exclude-filter";

    /**
     * Load the tests configuration that will be run. Each tests is defined by a {@link
     * IConfiguration} and a unique name under which it will report results. There are 2 ways to
     * load tests for {@link TestMappingSuiteRunner}:
     *
     * <p>1. --test-mapping-test-group, which specifies the group of tests in TEST_MAPPING files.
     * The runner will parse all TEST_MAPPING files in the source code through build artifact
     * test_mappings.zip, and load tests grouped under the given test group.
     *
     * <p>2. --include-filter, which specifies the name of the test to run. The use case is for
     * presubmit check to only run a list of tests related to the Cls to be verifies. The list of
     * tests are compiled from the related TEST_MAPPING files in modified source code.
     *
     * @return a map of test name to the {@link IConfiguration} object of each test.
     */
    @Override
    public LinkedHashMap<String, IConfiguration> loadTests() {
        // Map between test names and a list of test sources for each test.
        Map<String, List<String>> testsInTestMapping = new HashMap<>();

        Set<String> includeFilter = getIncludeFilter();
        if (mTestGroup == null && includeFilter.isEmpty()) {
            throw new RuntimeException(
                    "At least one of the options, --test-mapping-test-group or --include-filter, "
                            + "should be set.");
        }
        if (mTestGroup == null && !mKeywords.isEmpty()) {
            throw new RuntimeException(
                    "Must specify --test-mapping-test-group when applying --test-mapping-keyword.");
        }
        if (mTestGroup != null && !includeFilter.isEmpty()) {
            throw new RuntimeException(
                    "If options --test-mapping-test-group is set, option --include-filter should "
                            + "not be set.");
        }

        if (mTestGroup != null) {
            Set<TestInfo> testsToRun =
                    TestMapping.getTests(
                            getBuildInfo(), mTestGroup, getPrioritizeHostConfig(), mKeywords);
            if (testsToRun.isEmpty()) {
                throw new RuntimeException(
                        String.format("No test found for the given group: %s.", mTestGroup));
            }

            // Name of the tests
            Set<String> testNames = new HashSet<>();

            Set<String> mappingIncludeFilters = new HashSet<>();
            Set<String> mappingExcludeFilters = new HashSet<>();

            // module-arg options compiled from test options for each test.
            Set<String> moduleArgs = new HashSet<>();
            for (TestInfo test : testsToRun) {
                boolean hasIncludeFilters = false;
                for (TestOption option : test.getOptions()) {
                    switch (option.getName()) {
                            // Handle include and exclude filter at the suite level to hide each
                            // test runner specific implementation and option names related to filtering
                        case TEST_MAPPING_INCLUDE_FILTER:
                            hasIncludeFilters = true;
                            mappingIncludeFilters.add(
                                    String.format("%s %s", test.getName(), option.getValue()));
                            break;
                        case TEST_MAPPING_EXCLUDE_FILTER:
                            mappingExcludeFilters.add(
                                    String.format("%s %s", test.getName(), option.getValue()));
                            break;
                        default:
                            String moduleArg =
                                    String.format("%s:%s", test.getName(), option.getName());
                            if (option.getValue() != null && !option.getValue().isEmpty()) {
                                moduleArg = String.format("%s:%s", moduleArg, option.getValue());
                            }
                            moduleArgs.add(moduleArg);
                            break;
                    }
                }
                if (!hasIncludeFilters) {
                    testNames.add(test.getName());
                }
            }

            if (mappingIncludeFilters.isEmpty()) {
                setIncludeFilter(testNames);
            } else {
                mappingIncludeFilters.addAll(testNames);
                setIncludeFilter(mappingIncludeFilters);
            }
            if (!mappingExcludeFilters.isEmpty()) {
                setExcludeFilter(mappingExcludeFilters);
            }
            addModuleArgs(moduleArgs);

            for (TestInfo test : testsToRun) {
                List<String> testSources = null;
                // TODO(b/117880789): tests may not be grouped by name once that bug is fixed.
                // Update the dictionary with better keys.
                if (testsInTestMapping.containsKey(test.getName())) {
                    testSources = testsInTestMapping.get(test.toString());
                } else {
                    testSources = new ArrayList<String>();
                    testsInTestMapping.put(test.getName(), testSources);
                }
                testSources.addAll(test.getSources());
            }
        }

        LinkedHashMap<String, IConfiguration> testConfigs = super.loadTests();
        for (Map.Entry<String, IConfiguration> entry : testConfigs.entrySet()) {
            ConfigurationDescriptor configDescriptor =
                    entry.getValue().getConfigurationDescription();
            if (testsInTestMapping.containsKey(configDescriptor.getModuleName())) {
                configDescriptor.addMetaData(
                        TestMapping.TEST_SOURCES,
                        testsInTestMapping.get(configDescriptor.getModuleName()));
            }
        }

        return testConfigs;
    }
}
