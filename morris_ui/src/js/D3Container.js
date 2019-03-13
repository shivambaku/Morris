export default class D3Container {
    constructor(margin, padding, outer_width, outer_height) {
        this.margin = margin;
        this.padding = padding;
        this.outer_width = outer_width;
        this.outer_height = outer_height;
        this.inner_width = this.outer_width - margin.left - margin.right;
        this.inner_height = this.outer_height - margin.top - margin.bottom;
        this.width = this.inner_width - padding.left - padding.right;
        this.height = this.inner_height - padding.top - padding.bottom;
    };
}