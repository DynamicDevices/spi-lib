#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_mti_f32.c"

/*******************************************************************************
* Function Name: setUp
****************************************************************************//**
*
* The setUp function can contain anything you would like to run before each
* test.
*
* Note: Not used for these test.
*
*******************************************************************************/
void setUp(void)
{
}


/*******************************************************************************
* Function Name: tearDown
****************************************************************************//**
*
* The tearDown function can contain anything you would like to run after each
* test.
*
* Note: Not used for these test.
*
*******************************************************************************/
void tearDown(void)
{
}


#define DATA_SIZE (10)
#define DELTA     (0.00001F)

static const float32_t mti_test_sin[10] =
{
    +0.00000000, +0.58778524, +0.95105654, +0.95105654, +0.58778524,
    +0.00000000, -0.58778524, -0.95105654, -0.95105654, -0.58778524
};

static const float32_t mti_test_zeros[DATA_SIZE]=
{
    +0.00000000, +0.00000000, +0.00000000, +0.00000000, +0.00000000,
    +0.00000000, +0.00000000, +0.00000000, +0.00000000, +0.00000000
};

static const float32_t mti_test_v1[DATA_SIZE] =
{
    +0.00000000, -0.05877852, -0.09510566, -0.09510566, -0.05877852,
    -0.00000000, +0.05877852, +0.09510566, +0.09510566, +0.05877852
};

static const float32_t mti_test_v2[DATA_SIZE] =
{
    +0.00000000, -0.11167920, -0.18070075, -0.18070075, -0.11167920,
    -0.00000000, +0.11167920, +0.18070075, +0.18070075, +0.11167920
};

static const float32_t mti_test_v3[DATA_SIZE] =
{
    +0.00000000, -0.15341195, -0.24822576, -0.24822576, -0.15341195,
    -0.00000000, +0.15341195, +0.24822576, +0.24822576, +0.15341195
};

struct TestCase
{
    const float32_t* input;
    const float32_t* output;
};

static struct TestCase cases[] = { { mti_test_sin,   mti_test_sin },
                                   { mti_test_zeros, mti_test_v1  },
                                   { mti_test_v1,    mti_test_v2  },
                                   { mti_test_v2,    mti_test_v3  } };

static void check_tc(struct TestCase* tc, ifx_mti_inst_f32* inst)
{
    const float32_t alpha             = 0.1;
    float32_t       output[DATA_SIZE] = { -1 };
    ifx_mti_f32(inst, tc->input, output);

    for (uint32_t i = 0; i < DATA_SIZE; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(DELTA, tc->output[i], output[i]);
    }
}


static void check_tc_inline(struct TestCase* tc, ifx_mti_inst_f32* inst)
{
    float32_t in_out[DATA_SIZE];
    for (int i = 0; i < DATA_SIZE; i++)
    {
        in_out[i] = tc->input[i];
    }
    ifx_mti_f32(inst, in_out, in_out);

    for (uint32_t i = 0; i < DATA_SIZE; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(DELTA, tc->output[i], in_out[i]);
    }
}


/*******************************************************************************
* Function Name: test_mti_f32
****************************************************************************//**
* Description:
* Checks the validity of test_mti_f32 calculations
*
*******************************************************************************/
void test_mti_f32()
{
    const float32_t alpha = 0.1;

    float32_t history[DATA_SIZE]        = { 0 };
    float32_t history_inline[DATA_SIZE] = { 0 };

    ifx_mti_inst_f32 inst         = { DATA_SIZE, alpha, history };
    ifx_mti_inst_f32 inst_inplace = { DATA_SIZE, alpha, history_inline };

    for (int i_tc = 0; i_tc < sizeof(cases) / sizeof(cases[0]); i_tc++)
    {
        check_tc(cases + i_tc, &inst);
        check_tc_inline(cases + i_tc, &inst_inplace);
    }
}


void test_mti_f32_single()
{
    const float32_t alpha = 0.2;

    float32_t history[]        = { 1.0f };
    float32_t history_inline[] = { 1.0f };
    float32_t in[]             = { 2.0f };
    float32_t in_out[]         = { 2.0f };
    float32_t out[]            = { -1.0f };
    float32_t expected_history = 1.2f;
    float32_t expected_value   = 1.0f;

    ifx_mti_inst_f32 inst         = { 1, alpha, history };
    ifx_mti_inst_f32 inst_inplace = { 1, alpha, history_inline };

    ifx_mti_f32(&inst, in, out);
    TEST_ASSERT_FLOAT_WITHIN(DELTA, out[0], expected_value);
    TEST_ASSERT_FLOAT_WITHIN(DELTA, history[0], expected_history);

    ifx_mti_f32(&inst_inplace, in_out, in_out);
    TEST_ASSERT_FLOAT_WITHIN(DELTA, in_out[0], expected_value);
    TEST_ASSERT_FLOAT_WITHIN(DELTA, history_inline[0], expected_history);
}


void test_mti_f32_zero()
{
    const float32_t alpha = 0.2f;

    float32_t history[]        = {};
    float32_t history_inline[] = {};
    float32_t in[]             = {};
    float32_t in_out[]         = {};
    float32_t out[]            = {};

    ifx_mti_inst_f32 inst         = { 0, alpha, history };
    ifx_mti_inst_f32 inst_inplace = { 0, alpha, history_inline };

    ifx_mti_f32(&inst, in, out);
    ifx_mti_f32(&inst, in_out, in_out);
}
