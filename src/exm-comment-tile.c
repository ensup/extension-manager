#include "exm-comment-tile.h"

#include <text-engine/format/import.h>
#include <text-engine/ui/display.h>

#include "exm-rating.h"

struct _ExmCommentTile
{
    GtkWidget parent_instance;

    ExmComment *comment;

    GtkLabel *author;
    GtkLabel *author_badge;
    GtkLabel *date;
    ExmRating *rating;
    TextDisplay *display;
};

G_DEFINE_FINAL_TYPE (ExmCommentTile, exm_comment_tile, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_COMMENT,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

ExmCommentTile *
exm_comment_tile_new (ExmComment *comment)
{
    return g_object_new (EXM_TYPE_COMMENT_TILE,
                         "comment", comment,
                         NULL);
}

static void
exm_comment_tile_finalize (GObject *object)
{
    G_OBJECT_CLASS (exm_comment_tile_parent_class)->finalize (object);
}

static void
exm_comment_tile_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
    ExmCommentTile *self = EXM_COMMENT_TILE (object);

    switch (prop_id)
    {
    case PROP_COMMENT:
        g_value_set_object (value, self->comment);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
exm_comment_tile_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
    ExmCommentTile *self = EXM_COMMENT_TILE (object);

    switch (prop_id)
    {
    case PROP_COMMENT:
        self->comment = g_value_get_object (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
exm_comment_tile_constructed (GObject *object)
{
    ExmCommentTile *self = EXM_COMMENT_TILE (object);

    g_return_if_fail (EXM_IS_COMMENT (self->comment));

    TextFrame *frame;
    GDateTime *datetime;

    gchar *text, *date;
    g_object_get (self->comment,
                  "comment", &text,
                  "date", &date,
                  NULL);

    frame = format_parse_html (text);
    g_free (text);

    datetime = g_date_time_new_from_iso8601 (date, g_time_zone_new_utc ());
    g_free (date);

    g_object_set (self->display, "frame", frame, NULL);

    if (datetime != NULL)
    {
      gtk_label_set_label (self->date, g_date_time_format (datetime, "%x"));
      g_date_time_unref (datetime);
    }

    G_OBJECT_CLASS (exm_comment_tile_parent_class)->constructed (object);
}

static void
exm_comment_tile_class_init (ExmCommentTileClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = exm_comment_tile_finalize;
    object_class->get_property = exm_comment_tile_get_property;
    object_class->set_property = exm_comment_tile_set_property;
    object_class->constructed = exm_comment_tile_constructed;

    properties [PROP_COMMENT] =
        g_param_spec_object ("comment",
                             "Comment",
                             "Comment",
                             EXM_TYPE_COMMENT,
                             G_PARAM_READWRITE|G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties (object_class, N_PROPS, properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/mattjakeman/ExtensionManager/exm-comment-tile.ui");

    gtk_widget_class_bind_template_child (widget_class, ExmCommentTile, display);
    gtk_widget_class_bind_template_child (widget_class, ExmCommentTile, author);
    gtk_widget_class_bind_template_child (widget_class, ExmCommentTile, author_badge);
    gtk_widget_class_bind_template_child (widget_class, ExmCommentTile, date);
    gtk_widget_class_bind_template_child (widget_class, ExmCommentTile, rating);

    gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);
}

static void
exm_comment_tile_init (ExmCommentTile *self)
{
    g_type_ensure (EXM_TYPE_RATING);

    gtk_widget_init_template (GTK_WIDGET (self));
}
