library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.hivek_pkg.all;

entity hivek is
    port (
        clock : in std_logic;
        reset : in std_logic;
        din   : in hivek_in_t;
        dout  : out hivek_out_t
    );
end hivek;

architecture behavior of hivek is
    signal pipe_i : pipeline_in_t;
    signal pipe_o : pipeline_out_t;

    signal if_i : instruction_fetch_stage_in_t;
    signal if_o : instruction_fetch_stage_out_t;

    signal iexp_i : instruction_expansion_stage_in_t;
    signal iexp_o : instruction_expansion_stage_out_t;

    signal id_i : instruction_decode_stage_in_t;
    signal id_o : instruction_decode_stage_out_t;

    signal id2_i : instruction_decode2_stage_in_t;
    signal id2_o : instruction_decode2_stage_out_t;

    signal exec_i : execution_stage_in_t;
    signal exec_o : execution_stage_out_t;

    signal exec2_i : execution2_stage_in_t;
    signal exec2_o : execution2_stage_out_t;

    signal wb_i : writeback_stage_in_t;
    signal wb_o : writeback_stage_out_t;
begin
    -- icache
    dout.icache_addr <= pipe_o.icache_addr;
    pipe_i.icache_data <= din.icache_data;

    -- dcache
    dout.op0.dcache_data <= pipe_o.dcache_data_0; 
    dout.op1.dcache_data <= pipe_o.dcache_data_1; 

    dout.op0.dcache_addr <= pipe_o.dcache_addr_0;
    dout.op1.dcache_addr <= pipe_o.dcache_addr_1;

    dout.op0.dcache_wren <= pipe_o.dcache_wren_0;
    dout.op1.dcache_wren <= pipe_o.dcache_wren_1;

    pipe_i.dcache_data_0 <= din.op0.dcache_data;
    pipe_i.dcache_data_1 <= din.op1.dcache_data;


    if_i    <= pipe_o.if_i;
    iexp_i  <= pipe_o.iexp_i;
    id_i    <= pipe_o.id_i;
    id2_i   <= pipe_o.id2_i;
    exec_i  <= pipe_o.exec_i;
    exec2_i <= pipe_o.exec2_i;
    wb_i    <= pipe_o.wb_i;

    pipe_i.if_o    <= if_o;
    pipe_i.iexp_o  <= iexp_o;
    pipe_i.id_o    <= id_o;
    pipe_i.id2_o   <= id2_o;
    pipe_i.exec_o  <= exec_o;
    pipe_i.exec2_o <= exec2_o;
    pipe_i.wb_o    <= wb_o;


    pipeline_u : pipeline
    port map (
        clock => clock,
        reset => reset,
        din   => pipe_i,
        dout  => pipe_o
    );

    instruction_fetch_stage_u : instruction_fetch_stage
    port map (
        clock => clock,
        reset => reset,
        din   => if_i,
        dout  => if_o
    );

    instruction_expansion_stage_u : instruction_expansion_stage
    port map (
        din  => iexp_i,
        dout => iexp_o
    );

    instruction_decode_stage_u : instruction_decode_stage
    port map (
        clock => clock,
        reset => reset,
        din   => id_i,
        dout  => id_o
    );

    instruction_decode2_stage_u : instruction_decode2_stage
    port map (
        din  => id2_i,
        dout => id2_o
    );

    execution_stage_u : execution_stage
    port map (
        clock => clock,
        reset => reset,
        din   => exec_i,
        dout  => exec_o
    );

    execution2_stage_u : execution2_stage
    port map (
        din  => exec2_i,
        dout => exec2_o
    );

    writeback_stage_u : writeback_stage
    port map (
        din  => wb_i,
        dout => wb_o
    );
end behavior;
