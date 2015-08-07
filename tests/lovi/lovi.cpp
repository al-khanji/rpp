#include <rpp.hpp>

#include <QtCore>
#include <QtGui>
#include <QPainter>

#include <string>
#include <vector>

// #include <lovi/lovi>
namespace lovi {

struct non_copyable {
    non_copyable() = default;

    non_copyable(const non_copyable&) = delete;
    non_copyable(non_copyable &&) = delete;

    non_copyable &operator=(const non_copyable &) = delete;
    non_copyable &operator=(non_copyable &&) = delete;
};

struct size {
    std::size_t width {};
    std::size_t height {};

    size() = default;
    size(std::size_t w, std::size_t h) : width(w), height(h) {}
};

struct position {
    std::int_fast64_t x {};
    std::int_fast64_t y {};

    position() = default;
    position(std::int_fast64_t ax, std::int_fast64_t ay) : x(ax), y(ay) {}
};

struct color {
    std::string ss;

    color() = default;
    color(std::string s) : ss(s) {}
};

namespace scene {
    struct background {
        color _color;

        background() = default;
        background (color c) : _color(c) {}
    };

    struct node {
        position pos;
        size sz;
        std::vector<std::unique_ptr<node>> child_nodes;

        virtual ~node() {}

        node() = default;
        node(node&&) = default;

        template <class ChildNode, class... Tail, class = std::enable_if_t<std::is_base_of<node, ChildNode>::value>>
        node(ChildNode&& child, Tail... tail) : node(std::forward<Tail>(tail)...)
        {
            child_nodes.push_back(std::make_unique<ChildNode>(std::forward<ChildNode>(child)));
        }

        template <class... Tail>
        node(position &&p, Tail... tail) : node(std::forward<Tail>(tail)...)
        {
            pos = std::forward<position>(p);
        }

        template <class... Tail>
        node(size &&s, Tail... tail) : node(std::forward<Tail>(tail)...)
        {
            sz = std::forward<size>(s);
        }
    };

    struct root : node {
        using node::node;
    };

    struct rect : node {
        using node::node;
        rect(rect&&) = default;

        background fill;

        template <class... Tail>
        rect(background b, Tail... tail)
            : rect(std::forward<Tail>(tail)...)
        {
            fill = std::forward<background>(b);
        }
    };

    struct mouse_area : node {
        using node::node;
        mouse_area(mouse_area&&) = default;

        rpp::emitter<> on_clicked;

        template <class... EmitterArgs, class Callback, class... Tail>
        mouse_area(rpp::emitter<EmitterArgs...> mouse_area::* em, Callback cb, Tail... tail)
            : node(std::forward<Tail>(tail)...)
        {
            (*this.*em).connect(cb);
        }
    };
};

namespace qt {

class surface : public QWindow, public non_copyable
{
    Q_OBJECT

public:
    surface() : QWindow(), backingStore(this) {}
    QBackingStore backingStore;

    rpp::emitter<> on_surface_damaged;

    void paintScene(const scene::root &root) {
        if (!isExposed())
            return;

        const QRect rect(0, 0, width(), height());
        backingStore.beginPaint(rect);

        QPainter p(backingStore.paintDevice());
        paintSceneNode(p, root);

        backingStore.endPaint();
        backingStore.flush(rect);
    }

    void paintSceneNode(QPainter &painter, const scene::node &node) {
        painter.save();
        painter.translate(node.pos.x, node.pos.y);

        if (auto rect = dynamic_cast<const scene::rect *>(&node)) {
            QColor color(QString::fromStdString(rect->fill._color.ss));
            painter.fillRect(0, 0, node.sz.width, node.sz.height, color);
        }

        for (const auto &n : node.child_nodes)
            paintSceneNode(painter, *n);

        painter.restore();
    }

protected:
    void resizeEvent(QResizeEvent *) {
        backingStore.resize(size());
        if (isExposed())
            on_surface_damaged.fire();
    }

    void exposeEvent(QExposeEvent *) {
        if (isExposed())
            on_surface_damaged.fire();
    }
};

} // namespace qt

struct surface {
    struct title {
        std::string str;

        title() = default;
        title(std::string t) : str(std::move(t)) {}
    };

    std::unique_ptr<qt::surface> window = std::make_unique<qt::surface>();
    std::unique_ptr<scene::root> scene_root;

    surface() {
        std::puts("init surface");
    }

    void set_visible(bool b) {
        window->setVisible(b);
    }

    template <class... Tail>
    surface(title&& t, Tail... tail) : surface(std::forward<Tail>(tail)...) {
        window->setTitle(QString::fromStdString(t.str));
    }

    template <class... Tail>
    surface(size&& sh, Tail... tail) : surface(std::forward<Tail>(tail)...) {
        window->setWidth(sh.width);
        window->setHeight(sh.height);
    }

    template <class... Tail>
    surface(scene::root&& r, Tail... tail) : surface(std::forward<Tail>(tail)...) {
        if (!scene_root) {
            window->on_surface_damaged.connect([&] {
                if (scene_root) window->paintScene(*scene_root);
            });
        }

        scene_root = std::make_unique<scene::root>(std::move(r));
    }
};

} // namespace lovi

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    auto surface = lovi::surface {
        lovi::surface::title("Hello World"),
        lovi::size(800, 600),

        lovi::scene::root {
            lovi::scene::rect {
                lovi::scene::background(lovi::color("green")),
                lovi::size(400, 400),

                lovi::scene::rect {
                    lovi::scene::background(lovi::color("white")),
                    lovi::position(100, 100),
                    lovi::size(40, 40),
                    lovi::scene::mouse_area {
                        &lovi::scene::mouse_area::on_clicked, [] {
                            std::puts("white rect clicked!");}}},

                lovi::scene::rect {
                    lovi::scene::background(lovi::color("black")),
                    lovi::position(300, 300),
                    lovi::size(60, 60)}}}};

    surface.set_visible(true);

    QTimer::singleShot(500, &app, &QCoreApplication::quit);
    return app.exec();
}

#include "lovi.moc"
