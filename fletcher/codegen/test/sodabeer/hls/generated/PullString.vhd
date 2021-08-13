-- ==============================================================
-- RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
-- Version: 2018.3
-- Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
-- 
-- ===========================================================

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity PullString is
port (
    ap_clk : IN STD_LOGIC;
    ap_rst : IN STD_LOGIC;
    ap_start : IN STD_LOGIC;
    ap_done : OUT STD_LOGIC;
    ap_idle : OUT STD_LOGIC;
    ap_ready : OUT STD_LOGIC;
    buffer_dvalid_address0 : OUT STD_LOGIC_VECTOR (7 downto 0);
    buffer_dvalid_ce0 : OUT STD_LOGIC;
    buffer_dvalid_we0 : OUT STD_LOGIC;
    buffer_dvalid_d0 : OUT STD_LOGIC_VECTOR (0 downto 0);
    buffer_last_address0 : OUT STD_LOGIC_VECTOR (7 downto 0);
    buffer_last_ce0 : OUT STD_LOGIC;
    buffer_last_we0 : OUT STD_LOGIC;
    buffer_last_d0 : OUT STD_LOGIC_VECTOR (0 downto 0);
    buffer_data_V_address0 : OUT STD_LOGIC_VECTOR (7 downto 0);
    buffer_data_V_ce0 : OUT STD_LOGIC;
    buffer_data_V_we0 : OUT STD_LOGIC;
    buffer_data_V_d0 : OUT STD_LOGIC_VECTOR (7 downto 0);
    length_data_V : IN STD_LOGIC_VECTOR (31 downto 0);
    chars_V_dout : IN STD_LOGIC_VECTOR (9 downto 0);
    chars_V_empty_n : IN STD_LOGIC;
    chars_V_read : OUT STD_LOGIC );
end;


architecture behav of PullString is 
    constant ap_const_logic_1 : STD_LOGIC := '1';
    constant ap_const_logic_0 : STD_LOGIC := '0';
    constant ap_ST_fsm_state1 : STD_LOGIC_VECTOR (1 downto 0) := "01";
    constant ap_ST_fsm_state2 : STD_LOGIC_VECTOR (1 downto 0) := "10";
    constant ap_const_lv32_0 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000000";
    constant ap_const_lv32_1 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000001";
    constant ap_const_lv1_1 : STD_LOGIC_VECTOR (0 downto 0) := "1";
    constant ap_const_lv31_0 : STD_LOGIC_VECTOR (30 downto 0) := "0000000000000000000000000000000";
    constant ap_const_lv31_1 : STD_LOGIC_VECTOR (30 downto 0) := "0000000000000000000000000000001";
    constant ap_const_lv32_2 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000010";
    constant ap_const_lv32_9 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000001001";
    constant ap_const_lv1_0 : STD_LOGIC_VECTOR (0 downto 0) := "0";
    constant ap_const_boolean_1 : BOOLEAN := true;

    signal ap_CS_fsm : STD_LOGIC_VECTOR (1 downto 0) := "01";
    attribute fsm_encoding : string;
    attribute fsm_encoding of ap_CS_fsm : signal is "none";
    signal ap_CS_fsm_state1 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state1 : signal is "none";
    signal chars_V_blk_n : STD_LOGIC;
    signal ap_CS_fsm_state2 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state2 : signal is "none";
    signal tmp_fu_104_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal i_fu_109_p2 : STD_LOGIC_VECTOR (30 downto 0);
    signal ap_block_state2 : BOOLEAN;
    signal i_op_assign_reg_89 : STD_LOGIC_VECTOR (30 downto 0);
    signal tmp_4_fu_115_p1 : STD_LOGIC_VECTOR (63 downto 0);
    signal i_op_assign_cast_fu_100_p1 : STD_LOGIC_VECTOR (31 downto 0);
    signal ap_NS_fsm : STD_LOGIC_VECTOR (1 downto 0);


begin




    ap_CS_fsm_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_CS_fsm <= ap_ST_fsm_state1;
            else
                ap_CS_fsm <= ap_NS_fsm;
            end if;
        end if;
    end process;


    i_op_assign_reg_89_assign_proc : process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
                i_op_assign_reg_89 <= i_fu_109_p2;
            elsif (((ap_start = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state1))) then 
                i_op_assign_reg_89 <= ap_const_lv31_0;
            end if; 
        end if;
    end process;

    ap_NS_fsm_assign_proc : process (ap_start, ap_CS_fsm, ap_CS_fsm_state1, chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        case ap_CS_fsm is
            when ap_ST_fsm_state1 => 
                if (((ap_start = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state1))) then
                    ap_NS_fsm <= ap_ST_fsm_state2;
                else
                    ap_NS_fsm <= ap_ST_fsm_state1;
                end if;
            when ap_ST_fsm_state2 => 
                if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_0) and (ap_const_logic_1 = ap_CS_fsm_state2))) then
                    ap_NS_fsm <= ap_ST_fsm_state1;
                elsif ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then
                    ap_NS_fsm <= ap_ST_fsm_state2;
                else
                    ap_NS_fsm <= ap_ST_fsm_state2;
                end if;
            when others =>  
                ap_NS_fsm <= "XX";
        end case;
    end process;
    ap_CS_fsm_state1 <= ap_CS_fsm(0);
    ap_CS_fsm_state2 <= ap_CS_fsm(1);

    ap_block_state2_assign_proc : process(chars_V_empty_n, tmp_fu_104_p2)
    begin
                ap_block_state2 <= ((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0));
    end process;


    ap_done_assign_proc : process(ap_start, ap_CS_fsm_state1, chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((((ap_start = ap_const_logic_0) and (ap_const_logic_1 = ap_CS_fsm_state1)) or (not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_0) and (ap_const_logic_1 = ap_CS_fsm_state2)))) then 
            ap_done <= ap_const_logic_1;
        else 
            ap_done <= ap_const_logic_0;
        end if; 
    end process;


    ap_idle_assign_proc : process(ap_start, ap_CS_fsm_state1)
    begin
        if (((ap_start = ap_const_logic_0) and (ap_const_logic_1 = ap_CS_fsm_state1))) then 
            ap_idle <= ap_const_logic_1;
        else 
            ap_idle <= ap_const_logic_0;
        end if; 
    end process;


    ap_ready_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_0) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            ap_ready <= ap_const_logic_1;
        else 
            ap_ready <= ap_const_logic_0;
        end if; 
    end process;

    buffer_data_V_address0 <= tmp_4_fu_115_p1(8 - 1 downto 0);

    buffer_data_V_ce0_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            buffer_data_V_ce0 <= ap_const_logic_1;
        else 
            buffer_data_V_ce0 <= ap_const_logic_0;
        end if; 
    end process;

    buffer_data_V_d0 <= chars_V_dout(9 downto 2);

    buffer_data_V_we0_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            buffer_data_V_we0 <= ap_const_logic_1;
        else 
            buffer_data_V_we0 <= ap_const_logic_0;
        end if; 
    end process;

    buffer_dvalid_address0 <= tmp_4_fu_115_p1(8 - 1 downto 0);

    buffer_dvalid_ce0_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            buffer_dvalid_ce0 <= ap_const_logic_1;
        else 
            buffer_dvalid_ce0 <= ap_const_logic_0;
        end if; 
    end process;

    buffer_dvalid_d0 <= chars_V_dout(1 - 1 downto 0);

    buffer_dvalid_we0_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            buffer_dvalid_we0 <= ap_const_logic_1;
        else 
            buffer_dvalid_we0 <= ap_const_logic_0;
        end if; 
    end process;

    buffer_last_address0 <= tmp_4_fu_115_p1(8 - 1 downto 0);

    buffer_last_ce0_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            buffer_last_ce0 <= ap_const_logic_1;
        else 
            buffer_last_ce0 <= ap_const_logic_0;
        end if; 
    end process;

    buffer_last_d0 <= chars_V_dout(1 downto 1);

    buffer_last_we0_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            buffer_last_we0 <= ap_const_logic_1;
        else 
            buffer_last_we0 <= ap_const_logic_0;
        end if; 
    end process;


    chars_V_blk_n_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if (((tmp_fu_104_p2 = ap_const_lv1_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            chars_V_blk_n <= chars_V_empty_n;
        else 
            chars_V_blk_n <= ap_const_logic_1;
        end if; 
    end process;


    chars_V_read_assign_proc : process(chars_V_empty_n, ap_CS_fsm_state2, tmp_fu_104_p2)
    begin
        if ((not(((tmp_fu_104_p2 = ap_const_lv1_1) and (chars_V_empty_n = ap_const_logic_0))) and (tmp_fu_104_p2 = ap_const_lv1_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            chars_V_read <= ap_const_logic_1;
        else 
            chars_V_read <= ap_const_logic_0;
        end if; 
    end process;

    i_fu_109_p2 <= std_logic_vector(unsigned(i_op_assign_reg_89) + unsigned(ap_const_lv31_1));
    i_op_assign_cast_fu_100_p1 <= std_logic_vector(IEEE.numeric_std.resize(unsigned(i_op_assign_reg_89),32));
    tmp_4_fu_115_p1 <= std_logic_vector(IEEE.numeric_std.resize(unsigned(i_op_assign_reg_89),64));
    tmp_fu_104_p2 <= "1" when (signed(i_op_assign_cast_fu_100_p1) < signed(length_data_V)) else "0";
end behav;
