import React, { Component } from 'react';
import { Dropdown, DropdownToggle, DropdownMenu, DropdownItem, Button } from 'reactstrap';
import autobind from 'auto-bind'
import * as d3 from 'd3'
import D3Container from './D3Container'
import addon from "../../build/Release/addon";

export default class NineMenMorrisUI extends Component {

  constructor() {
    super();
    autobind.react(this);

    this.state = {
      game_state: undefined,
      on_going: true,
      left_player_ai_drop_down_open: false,
      right_player_ai_drop_down_open: false,
      left_player_ai_name: 'Human',
      right_player_ai_name: 'Human',
      moves: [],
      placement_moves: [],
      deletion_moves: []
    };

    this.left_player = 'left_player';
    this.right_player = 'right_player';
    this.ais = ['Human', 'AI'];

    this.stage = {
      kPlacement: 0,
      kMovement: 1,
      kFreeMovement: 2
    };

    this.coordinates = this.buildCoordinates();
  }

  componentDidMount() {
    this.initialize();
    this.createInfoBoard();
    this.createBoard();
  }

  initialize() {
    this.simulation = addon.Simulation();
    this.setState({ on_going: true, moves: [], placement_moves: [], deletion_moves: [] });
    this.setGameState();
  }

  reset() {
    this.initialize();
  }

  step() {
    if (!this.state.on_going || this.getCurrentPlayerAI() == "Human") return;

    let on_going = this.simulation.Move();
    this.setState({ on_going: on_going });

    this.setGameState();
  }

  setGameState() {
    let state = this.simulation.State();
    console.log(state);
    this.setState({ game_state: state });

    let game_moves = this.simulation.ListMoves();

    let unique_sources = new Set();
    let game_source_moves = [];
    if (this.getCurrentPlayerAI(state) === "Human") {
      for (let i = 0; i < game_moves.length; ++i) {
        if (game_moves[i].source != -1 && !unique_sources.has(game_moves[i].source)) {
          game_source_moves.push(game_moves[i]);
          unique_sources.add(game_moves[i].source);
        }
      }
    }

    let unique_destinations = new Set();
    let game_placement_moves = [];
    if (this.getCurrentPlayerAI(state) === "Human") {
      for (let i = 0; i < game_moves.length; ++i) {
        if (game_moves[i].source == -1 && !unique_destinations.has(game_moves[i].destination)) {
          game_placement_moves.push(game_moves[i]);
          unique_destinations.add(game_moves[i].destination);
        }
      }
    }

    this.setState({
      moves: game_moves,
      source_moves: game_source_moves,
      placement_moves: game_placement_moves
    });
  }

  buildCoordinates() {
    return [
      { x: 0, y: 0 }, { x: 3, y: 0 }, { x: 6, y: 0 },
      { x: 1, y: 1 }, { x: 3, y: 1 }, { x: 5, y: 1 },
      { x: 2, y: 2 }, { x: 3, y: 2 }, { x: 4, y: 2 },
      { x: 0, y: 3 }, { x: 1, y: 3 }, { x: 2, y: 3 }, { x: 4, y: 3 }, { x: 5, y: 3 }, { x: 6, y: 3 },
      { x: 2, y: 4 }, { x: 3, y: 4 }, { x: 4, y: 4 },
      { x: 1, y: 5 }, { x: 3, y: 5 }, { x: 5, y: 5 },
      { x: 0, y: 6 }, { x: 3, y: 6 }, { x: 6, y: 6 }
    ];
  }

  coordinateToWorldSpace(coordinates, size) {
    let world_space_coordinates = [];
    for (let i = 0; i < coordinates.length; ++i) {
      world_space_coordinates.push({ x: coordinates[i].x * size / 6.0, y: coordinates[i].y * size / 6.0 });
    }
    return world_space_coordinates;
  }

  getCurrentPlayerAI(state) {
    state = state === undefined ? this.state.game_state : state;
    if (state.player == this.left_player) {
      return this.state.left_player_ai_name;
    } else {
      return this.state.right_player_ai_name;
    }
  }

  createInfoBoard() {
    let margin = { top: 10, right: 10, bottom: 10, left: 10 };
    let padding = { top: 100, right: 100, bottom: 100, left: 100 };

    this.container_info = new D3Container(margin, padding, 500, 500);

    let svg = d3.select(this.refs.div_info_board)
      .append('svg')
      .attr('width', this.container_info.outer_width)
      .attr('height', this.container_info.outer_height)
      .append('g')
      .attr('transform', `translate(${margin.left}, ${margin.top})`);

    let g = svg.append('g')
      .attr('id', 'info_group')
      .attr('transform', `translate(${padding.left}, ${padding.top})`);

    g.append('rect')
      .attr('width', this.container_info.width)
      .attr('height', this.container_info.height)
      .attr('fill', 'lightgrey')
      .style('opacity', 0.5);

    g.append('text')
      .attr('class', 'turn_text')
      .attr('x', this.container_info.width / 2.0)
      .attr('y', this.container_info.height * 1.0 / 4.0)
      .attr('text-anchor', 'middle')
      .attr('alignment-baseline', 'middle')
      .attr('font-size', '22px');

    g.append('text')
      .attr('class', 'left_remaining_pieces_text')
      .attr('x', this.container_info.width / 2.0)
      .attr('y', this.container_info.height * 2.0 / 4.0)
      .attr('text-anchor', 'middle')
      .attr('alignment-baseline', 'middle')
      .attr('font-size', '22px');

    g.append('text')
      .attr('class', 'right_remaining_pieces_text')
      .attr('x', this.container_info.width / 2.0)
      .attr('y', this.container_info.height * 3.0 / 4.0)
      .attr('text-anchor', 'middle')
      .attr('alignment-baseline', 'middle')
      .attr('font-size', '22px');
  }

  renderInfoBoard() {

    let g = d3.select(this.refs.div_info_board).select('svg').select('#info_group');

    g.select('.turn_text')
      .text(this.state.game_state.player === this.left_player ? "Black's Turn" : "White's Turn");

    g.select('.left_remaining_pieces_text')
      .text(`Black Remaining: ${this.state.game_state.left_player_remaining}`);

    g.select('.right_remaining_pieces_text')
      .text(`White Remaining: ${this.state.game_state.right_player_remaining}`);
  }

  createBoard() {

    let self = this;

    // define the screen size of the board
    let margin = { top: 10, right: 10, bottom: 10, left: 10 };
    let padding = { top: 50, right: 50, bottom: 50, left: 50 };

    this.container = new D3Container(margin, padding, 500, 500);

    // create svg and translate by the margin
    let svg = d3.select(this.refs.board_div)
      .append('svg')
      .attr('width', this.container.outer_width)
      .attr('height', this.container.outer_height)
      .append('g')
      .attr('transform', `translate(${margin.left}, ${margin.top})`);

    // group that will contain all the content
    let g = svg.append('g')
      .attr('id', 'board_group')
      .attr('transform', `translate(${padding.left}, ${padding.top})`);

    let size = Math.min(this.container.width, this.container.height);

    this.scale = d3.scaleLinear().domain([0, 6]).range([0, size]).nice();
    this.cell_size = this.scale(1);

    let stroke_width = 3;
    let stroke_color = 'grey';

    g.append('rect')
      .attr('width', size)
      .attr('height', size)
      .attr('fill', 'none')
      .attr('stroke', stroke_color)
      .attr('stroke-width', stroke_width);

    g.append('rect')
      .attr('x', size / 6.0)
      .attr('y', size / 6.0)
      .attr('width', size * 4.0 / 6.0)
      .attr('height', size * 4.0 / 6.0)
      .attr('fill', 'none')
      .attr('stroke', stroke_color)
      .attr('stroke-width', stroke_width);

    g.append('rect')
      .attr('x', size * 2.0 / 6.0)
      .attr('y', size * 2.0 / 6.0)
      .attr('width', size * 2.0 / 6.0)
      .attr('height', size * 2.0 / 6.0)
      .attr('fill', 'none')
      .attr('stroke', stroke_color)
      .attr('stroke-width', stroke_width);

    g.append('line')
      .attr('x1', this.scale(this.coordinates[1].x))
      .attr('y1', this.scale(this.coordinates[1].y))
      .attr('x2', this.scale(this.coordinates[7].x))
      .attr('y2', this.scale(this.coordinates[7].y))
      .attr('stroke', stroke_color)
      .attr('stroke-width', stroke_width);

    g.append('line')
      .attr('x1', this.scale(this.coordinates[9].x))
      .attr('y1', this.scale(this.coordinates[9].y))
      .attr('x2', this.scale(this.coordinates[11].x))
      .attr('y2', this.scale(this.coordinates[11].y))
      .attr('stroke', stroke_color)
      .attr('stroke-width', stroke_width);

    g.append('line')
      .attr('x1', this.scale(this.coordinates[12].x))
      .attr('y1', this.scale(this.coordinates[12].y))
      .attr('x2', this.scale(this.coordinates[14].x))
      .attr('y2', this.scale(this.coordinates[14].y))
      .attr('stroke', stroke_color)
      .attr('stroke-width', stroke_width);

    g.append('line')
      .attr('x1', this.scale(this.coordinates[16].x))
      .attr('y1', this.scale(this.coordinates[16].y))
      .attr('x2', this.scale(this.coordinates[22].x))
      .attr('y2', this.scale(this.coordinates[22].y))
      .attr('stroke', stroke_color)
      .attr('stroke-width', stroke_width);

    let decorCircles = g.selectAll('.decorCircle').data(this.coordinates);

    decorCircles.enter()
      .append('circle')
      .attr('class', 'decorCircle')
      .attr('r', '5')
      .attr('cx', d => this.scale(d.x))
      .attr('cy', d => this.scale(d.y))
      .attr('fill', 'black');

    decorCircles.exit().remove();
  }

  renderBoard() {
    let self = this;

    let g = d3.select(this.refs.board_div).select('svg').select('#board_group');

    let pieces = g.selectAll('.piece').data(this.state.game_state.board);

    pieces.enter()
      .append('circle')
      .attr('class', 'piece')
      .attr('r', this.cell_size / 4.0)
      .style('stroke', 'black')
      .attr('cx', (d, i) => this.scale(this.coordinates[i].x))
      .attr('cy', (d, i) => this.scale(this.coordinates[i].y))
      .merge(pieces)
      .style('fill', d => d == this.left_player ? 'black' : 'white')
      .style('opacity', d => d == 'empty' ? 0 : 1.0);

    pieces.exit().remove();

    let source_circles = g.selectAll('.source_circle').data(this.state.source_moves);

    source_circles.enter()
      .append('circle')
      .attr('class', 'source_circle')
      .attr('r', '25')
      .style('stroke', 'black')
      .style('fill', 'grey')
      .style('opacity', '0.2')
      .merge(source_circles)
      .attr('cx', d => this.scale(this.coordinates[d.source].x))
      .attr('cy', d => this.scale(this.coordinates[d.source].y))
      .on('click', function (d) {
        if (self.getCurrentPlayerAI() == "Human" &&
          self.state.on_going == true) {
          self.setState({ source_moves: [], placement_moves: self.state.moves.filter(x => x.source === d.source) });
        }
      });

    source_circles.exit().remove();

    let placement_circles = g.selectAll('.placement_circle').data(this.state.placement_moves);

    placement_circles.enter()
      .append('circle')
      .attr('class', 'placement_circle')
      .attr('r', '20')
      .style('stroke', 'black')
      .style('fill', 'grey')
      .style('opacity', '0.2')
      .merge(placement_circles)
      .attr('cx', d => this.scale(this.coordinates[d.destination].x))
      .attr('cy', d => this.scale(this.coordinates[d.destination].y))
      .on('click', function (d) {
        if (self.getCurrentPlayerAI() == "Human" &&
          self.state.on_going == true) {

          if (d.deletion != -1) {
            let next_game_state = self.state.game_state;

            if (d.source != -1) {
              next_game_state.board[d.source] = 'empty';
            }
            next_game_state.board[d.destination] = self.state.game_state.player;

            self.setState({ game_state: next_game_state });
            self.setState({
              placement_moves: [],
              deletion_moves: self.state.moves.filter(x => x.source == d.source && x.destination == d.destination && x.deletion != -1)
            });
          } else {
            console.log(d);
            let on_going = self.simulation.MoveHuman(d);
            self.setState({ on_going: on_going });
            self.setGameState();
          }
        }
      });

    placement_circles.exit().remove();

    let deletion_circles = g.selectAll('.deletion_circle').data(this.state.deletion_moves);

    deletion_circles.enter()
      .append('circle')
      .attr('class', 'deletion_circle')
      .attr('r', '20')
      .style('stroke', 'black')
      .style('fill', 'red')
      .style('opacity', '0.2')
      .merge(deletion_circles)
      .attr('cx', d => this.scale(this.coordinates[d.deletion].x))
      .attr('cy', d => this.scale(this.coordinates[d.deletion].y))
      .on('click', function (d) {
        console.log(d);
        let on_going = self.simulation.MoveHuman(d);
        self.setState({ on_going: on_going });
        self.setState({ deletion_moves: [] });
        self.setGameState();
      });

    deletion_circles.exit().remove();
  }

  changeAlgorithm(player, ai_name) {
    if (player === this.left_player) {
      this.setState({ left_player_ai_name: ai_name });
    } else {
      this.setState({ right_player_ai_name: ai_name });
    }
  }

  toggleLeftPlayerAiDropDown() {
    this.setState(prevState => ({
      left_player_ai_drop_down_open: !prevState.left_player_ai_drop_down_open
    }));
  }

  toggleRightPlayerAiDropDown() {
    this.setState(prevState => ({
      right_player_ai_drop_down_open: !prevState.right_player_ai_drop_down_open
    }));
  }

  render() {
    if (this.state.game_state != undefined) {
      this.renderBoard();
      this.renderInfoBoard();
    }

    let left_player_options = this.ais.map(ai_name =>
      <DropdownItem key={ai_name} onClick={() => this.changeAlgorithm(this.left_player, ai_name)}>
        {ai_name}
      </DropdownItem>
    );

    let right_player_options = this.ais.map(ai_name =>
      <DropdownItem key={ai_name} onClick={() => this.changeAlgorithm(this.right_player, ai_name)}>
        {ai_name}
      </DropdownItem>
    );

    return (
      <div>
        <div ref='board_div' style={{ float: 'left' }} />
        <div ref='div_info_board' />
        <div className="row">
          <button className="btn btn-info" onClick={this.step}>
            Step
          </button>
          &nbsp;
          <Dropdown isOpen={this.state.right_player_ai_drop_down_open}
            toggle={this.toggleRightPlayerAiDropDown}>
            <DropdownToggle color="secondary" caret>
              {this.state.right_player_ai_name}
            </DropdownToggle>
            <DropdownMenu>
              {right_player_options}
            </DropdownMenu>
          </Dropdown>
          &nbsp;
          <Dropdown isOpen={this.state.left_player_ai_drop_down_open}
            toggle={this.toggleLeftPlayerAiDropDown}>
            <DropdownToggle color="secondary" caret>
              {this.state.left_player_ai_name}
            </DropdownToggle>
            <DropdownMenu>
              {left_player_options}
            </DropdownMenu>
          </Dropdown>
          &nbsp;
          <button className="btn btn-warning" onClick={this.reset}>
            Reset
          </button>
        </div>
      </div>
    );
  }
}